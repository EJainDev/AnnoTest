import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from support import RunnerTest

class ProcessTests(RunnerTest):
    def check(self,mode,expected):
        with tempfile.TemporaryDirectory() as directory:
            result=subprocess.run([str(self.binary_dir/'ProcessFaultTests'),mode,directory],capture_output=True,text=True,timeout=5)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            self.assertIn(expected,result.stdout)
            if mode=='wait-interrupt':
                self.assertEqual((Path(directory)/'stdout').read_text(),'child output\n')
                self.assertEqual((Path(directory)/'stderr').read_text(),'child error\n')

for mode,message in (('empty','Missing executable'),('files-init','Cannot configure worker output'),
                     ('open-first','Cannot redirect worker output'),('open-second','Cannot redirect worker output'),
                     ('attributes','Cannot configure worker process'),('missing','Cannot start test worker'),
                     ('wait-error','Cannot wait for test worker'),('wait-error-interrupt','Cannot wait for test worker'),('wait-error-terminal','Cannot wait for test worker'),('wait-interrupt',''),
                     ('hang','Test deadline exceeded'),('timeout-interrupt','Test deadline exceeded'),('timeout-terminal','Test deadline exceeded')):
    setattr(ProcessTests,'test_'+mode.replace('-','_'),lambda self,m=mode,e=message:self.check(m,e))
