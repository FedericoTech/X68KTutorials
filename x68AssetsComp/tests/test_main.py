import unittest
import tempfile
import os
import filecmp
from unittest.mock import patch
import main  # Import your main CLI module


class TestTiletoolCLI(unittest.TestCase):

    def setUp(self):
        # Create a temporary directory for the test
        self.test_dir = tempfile.TemporaryDirectory()
        # Path to the samples directory
        self.samples_dir = os.path.join(os.path.dirname(__file__), 'samples')

    def tearDown(self):
        # Clean up the temporary directory
        self.test_dir.cleanup()

    def test_convert_tmx(self):
        output_dir = self.test_dir.name
        input_file = os.path.join(self.samples_dir, 'tiled.tmx')
        test_args = ['convert-tmx', '--input', input_file, '--output', output_dir]
        with patch('sys.argv', ['main.py'] + test_args):
            main.main()

        expected_pal = os.path.join(self.samples_dir, 'tiles.pal')
        expected_png = os.path.join(self.samples_dir, 'tiles.png')
        expected_tm = os.path.join(self.samples_dir, 'tiles.tm')
        expected_ts = os.path.join(self.samples_dir, 'tiles.ts')

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "tiles.pal"),
            expected_pal,
            False
        ), "Generated pal file does not match the expected file")

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "tiles.tm"),
            expected_tm,
            False
        ), "Generated tm file does not match the expected file")

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "tiles.ts"),
            expected_ts,
            False
        ), "Generated ts file does not match the expected file")

    def test_convert_image_4bits(self):
        input_file = os.path.join(self.samples_dir, 'formula4b.png')
        output_dir = self.test_dir.name
        #output_file = os.path.join(os.path.dirname(__file__), 'ello')
        expected_pic = os.path.join(self.samples_dir, 'formula4b.pic')
        expected_pal = os.path.join(self.samples_dir, 'formula4b.pal')

        test_args = ['convert-image', '--input', input_file, '--output', output_dir, '--format', '4bits']
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

    def test_convert_image_8bits(self):
        input_file = os.path.join(self.samples_dir, '8BitsColors.bmp')
        output_dir = self.test_dir.name
        #output_file = os.path.join(os.path.dirname(__file__), 'ello')
        expected_pic = os.path.join(self.samples_dir, '8BitsColors.pic')
        expected_pal = os.path.join(self.samples_dir, '8BitsColors.pal')

        test_args = ['convert-image', '--input', input_file, '--output', output_dir, '--format', '8bits']
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

    def test_convert_image_16bits(self):
        input_file = os.path.join(self.samples_dir, '16BitLandSc.bmp')
        output_dir = self.test_dir.name
        #output_file = os.path.join(os.path.dirname(__file__), 'ello')
        expected_pic = os.path.join(self.samples_dir, '16BitLandSc.pic')

        test_args = ['convert-image', '--input', input_file, '--output', output_dir, '--format', '16bits']
        with patch('sys.argv', ['main.py'] + test_args):
            main.main()

        self.assertTrue(filecmp.cmp(
            os.path.join(output_dir, "16BitLandSc.pic"),
            expected_pic,
            False
        ), "Generated pic file does not match the expected file")

    def test_convert_audio(self):
        output_dir = os.path.join(self.test_dir.name, 'output.pcm')
        expected_file = os.path.join(self.samples_dir, 'expected_audio_pcm16.pcm')
        input_file = os.path.join(self.samples_dir, 'music.mp3')

        test_args = ['convert-audio', '--input', input_file, '--output', output_dir, '--format', 'pcm16']
        with patch('sys.argv', ['main.py'] + test_args):
            main.main()

        self.assertTrue(filecmp.cmp(output_dir, expected_file), "Generated file does not match the expected file")


if __name__ == '__main__':
    unittest.main()
