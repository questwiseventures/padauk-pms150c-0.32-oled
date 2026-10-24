"""Portable host release checks only; no MCU compilation/hardware."""
from pathlib import Path
import hashlib,json,re,sys,tempfile,unittest,contextlib,io
P=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(P/'converter'))
from png_to_minic import main,validate_loader_name
class ReleaseTests(unittest.TestCase):
    def test_source_provenance(self):
        record=json.loads((P/'PROVENANCE.json').read_text())
        for name,info in record['copied_files'].items():
            self.assertEqual(hashlib.sha256((P/name).read_bytes()).hexdigest(),info['sha256'],name)
    def test_default_project_and_api_freeze(self):
        app=P/'application_template'
        code=(app/'PMS150C_OLED_APP.C').read_text(encoding='ascii')
        self.assertNotIn('DEMO_',code)
        self.assertIn('NUMBER_VALUE = 37;',code)
        flags=(app/'feature_config.h').read_text().split('// Internal derived')[0]
        enabled={k for k,v in re.findall(r'(OLED_\w+) => ([01]);',flags) if v=='1'}
        self.assertEqual(enabled,{'OLED_USE_DRAWNUMBER','OLED_USE_DRAWCHAR'})
        lib=(app/'oled_library.h').read_text(encoding='ascii')
        self.assertEqual(lib.count('BYTE CELL_DATA[8];'),1)
        self.assertIn('.delay 400000;',lib)
        for name in json.loads((P/'PROVENANCE.json').read_text())['frozen_public_functions']:
            self.assertRegex(lib,r'void\s+'+name+r'\s*\(void\)')
        for ref in re.findall(r'^~([^\r\n]+)',(app/'PMS150C_OLED_APP.PRJ').read_text(),re.M):
            if not ref.startswith('$:'):self.assertTrue((app/ref).exists(),ref)
    def test_converter_matches_packaged_header(self):
        with tempfile.TemporaryDirectory() as folder:
            output=Path(folder)/'image.h';meta=Path(folder)/'tiles.json'
            args=[str(P/'assets/pizza_32x32.png'),'--format','template-header','--loader-name','Image_LoadTile','--foreground','white','--output',str(output),'--metadata',str(meta)]
            self.assertEqual(main(args),0)
            self.assertEqual(output.read_bytes(),(P/'application_template/bitmap_image.h').read_bytes())
            self.assertEqual(json.loads(meta.read_text())['tiles'],json.loads((P/'assets/pizza.tiles.json').read_text())['tiles'])
            with contextlib.redirect_stderr(io.StringIO()):
                with self.assertRaises(SystemExit):main(args)
            with self.assertRaises(ValueError):validate_loader_name('CELL_DATA')
    def test_portable_doc_links(self):
        for file in P.rglob('*.md'):
            text=file.read_text(encoding='utf-8')
            self.assertNotIn('C:/Dropbox',text)
            for target in re.findall(r'\]\(([^)]+)\)',text):
                if '://' not in target:self.assertTrue((file.parent/target.split('#')[0]).exists(),str(file)+':'+target)
    def test_application_examples(self):
        minimal=P/'examples/minimal_character'
        cube=P/'examples/cube_loading'
        code=(minimal/'PMS150C_OLED_MINIMAL.C').read_text(encoding='ascii')
        app=code[code.index('// BEGIN APPLICATION DRAWING'):]
        self.assertIn('A = 0x30;',app)
        self.assertEqual(re.findall(r'\b(\w+)\(\);',app),['setCursor','drawChar'])
        self.assertNotIn('NUMBER_VALUE',app);self.assertNotIn('DEMO_',code)
        for folder,basename in ((minimal,'PMS150C_OLED_MINIMAL'),(cube,'PMS150C_OLED_PERCENT_CUBE')):
            self.assertEqual((folder/'oled_library.h').read_bytes(),(P/'application_template/oled_library.h').read_bytes())
            for ref in re.findall(r'^~([^\r\n]+)',(folder/(basename+'.PRJ')).read_text(),re.M):
                if not ref.startswith('$:'):self.assertTrue((folder/ref).exists(),ref)
        font=(minimal/'font_config.h').read_text()
        self.assertIn('FONT_30 => 1;',font);self.assertIn('FONT_DIGITS => 0;',font)
        self.assertIn('FONT_SPACE => 0;',font);self.assertIn('FONT_25 => 0;',font)
        cubecode=(cube/'PMS150C_OLED_PERCENT_CUBE.C').read_text()
        self.assertIn('drawPercentBar();',cubecode)
        self.assertNotIn('A = 0x25;',cubecode)

    def test_no_build_outputs(self):
        for file in P.rglob('*'):
            self.assertNotIn(file.suffix.upper(),{'.PDK','.TMP','.OBJ'})
            self.assertNotIn('OBJ',file.parts)
if __name__=='__main__':unittest.main()
