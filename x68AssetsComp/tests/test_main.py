import unittest
import tempfile
import os
import filecmp
from io import StringIO
import sys
from unittest.mock import patch
from x68assetscomp import main  # Import your main CLI module


class TestTiletoolCLI(unittest.TestCase):

    def setUp(self):
        # Create a temporary directory for the test
        self.test_dir = tempfile.TemporaryDirectory()
        # Path to the samples directory
        self.samples_dir = os.path.join(os.path.dirname(__file__), 'samples')

        self.original_stdout = sys.stdout
        self.output = StringIO()  # Initialize StringIO object to capture printed output
        sys.stdout = self.output  # Redirect stdout to StringIO object

    def tearDown(self):
        # Clean up the temporary directory
        self.test_dir.cleanup()
        sys.stdout = self.original_stdout  # Restore original stdout

    def test_convert_tmx(self):
        output_dir = self.test_dir.name
        input_file = os.path.join(self.samples_dir, 'tiled.tmx')
        test_args = ['convert-tmx', '--input', input_file, '--output_dir', output_dir, '--output_name', 'mk_circuit']
        with patch('sys.argv', ['main.py'] + test_args):
            main.main()

        expected_pal = os.path.join(self.samples_dir, 'mk_circuit.pal')
        expected_tm = os.path.join(self.samples_dir, 'mk_circuit.tm')
        expected_ts = os.path.join(self.samples_dir, 'mk_circuit.ts')

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "mk_circuit.pal"),
            expected_pal,
            False
        ), "Generated pal file does not match the expected file")

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "mk_circuit.tm"),
            expected_tm,
            False
        ), "Generated tm file does not match the expected file")

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "mk_circuit.ts"),
            expected_ts,
            False
        ), "Generated ts file does not match the expected file")

        # Capture the printed output
        printed_output = self.output.getvalue().strip()

        # Assert that the warning message is in the printed output
        self.assertIn("\033[93mWarning: \033[0mThe output name is more than 8 characters long.", printed_output)

    def test_convert_image_4bits(self):
        input_file = os.path.join(self.samples_dir, 'formula4b.png')
        output_dir = self.test_dir.name
        #output_file = os.path.join(os.path.dirname(__file__), 'ello')
        expected_pic = os.path.join(self.samples_dir, 'formula4b.pic')
        expected_pal = os.path.join(self.samples_dir, 'formula4b.pal')

        test_args = ['convert-image', '--input', input_file, '--output_dir', output_dir, '--format', '4bits',
                     '--no-warn']
        with patch('sys.argv', ['main.py'] + test_args):
            main.main()

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "formula4b.pic"),
            expected_pic,
            False
        ), "Generated pic file does not match the expected file")

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "formula4b.pal"),
            expected_pal,
            False
        ), "Generated pal file does not match the expected file")

        # Capture the printed output
        printed_output = self.output.getvalue().strip()

        # Assert that the warning message is in the printed output
        self.assertNotIn("\033[93mWarning: \033[0mThe output name is more than 8 characters long.", printed_output)

    def test_convert_image_8bits(self):
        input_file = os.path.join(self.samples_dir, '8BitsColors.bmp')
        output_dir = self.test_dir.name
        #output_file = os.path.join(os.path.dirname(__file__), 'ello')
        expected_pic = os.path.join(self.samples_dir, '8BitsColors.pic')
        expected_pal = os.path.join(self.samples_dir, '8BitsColors.pal')

        test_args = ['convert-image', '--input', input_file, '--output_dir', output_dir, '--format', '8bits']
        with patch('sys.argv', ['main.py'] + test_args):
            main.main()

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "8BitsColors.pic"),
            expected_pic,
            False
        ), "Generated pic file does not match the expected file")

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "8BitsColors.pal"),
            expected_pal,
            False
        ), "Generated pal file does not match the expected file")

        # Capture the printed output
        printed_output = self.output.getvalue().strip()

        # Assert that the warning message is in the printed output
        self.assertIn("\033[93mWarning: \033[0mThe output name is more than 8 characters long.", printed_output)

    def test_convert_image_16bits(self):
        input_file = os.path.join(self.samples_dir, '16BitLandSc.bmp')
        output_dir = self.test_dir.name
        #output_file = os.path.join(os.path.dirname(__file__), 'ello')
        expected_pic = os.path.join(self.samples_dir, '16BitLandSc.pic')

        test_args = ['convert-image', '--input', input_file, '--output_dir', output_dir, '--format', '16bits',
                     '--output_name', '16BitLandSc']
        with patch('sys.argv', ['main.py'] + test_args):
            main.main()

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "16BitLandSc.pic"),
            expected_pic,
            False
        ), "Generated pic file does not match the expected file")

        # Capture the printed output
        printed_output = self.output.getvalue().strip()

        # Assert that the warning message is in the printed output
        self.assertIn("\033[93mWarning: \033[0mThe output name is more than 8 characters long.", printed_output)

    def test_convert_audio(self):
        input_file = os.path.join(self.samples_dir, 'LWT11.wav')
        output_dir = self.test_dir.name
        expected_file = os.path.join(self.samples_dir, 'LWT11_15_6khz.raw')

        test_args = ['convert-audio', '--input', input_file, '--output_dir', output_dir, '--output_name', 'LWT11_15_6khz',
                     '--sample', '5']
        with patch('sys.argv', ['main.py'] + test_args):
            main.main()

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, 'LWT11_15_6khz.raw'),
            expected_file,
            False
        ), "Generated file does not match the expected file")


if __name__ == '__main__':
    unittest.main()
