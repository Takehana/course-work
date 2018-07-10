import unittest
import subprocess
import requests

PORT=8080
class TestHW1(unittest.TestCase):
    
    def test_everything(self):
        #output = subprocess.check_output('curl -X GET http://localhost:'+str(PORT)+'/hello',shell=True)
        res = requests.get('http://localhost:'+str(PORT)+'/hello')
        self.assertEqual(res.text, 'Hello world!', msg='GET on the hello resource did not execute successfully')
                
        #output = subprocess.check_output(['curl', '-X', 'GET', 'http://localhost:'+str(PORT)+'/echo'])
        res = requests.get('http://localhost:'+str(PORT)+'/echo')
        self.assertEqual(res.text, '', msg='GET echo without a msg =. Should have output a blank.')
                
        #output = subprocess.check_output(['curl', '-X', 'GET', 'http://localhost:'+str(PORT)+'/echo?msg=AnyColourYouLike'])
        res = requests.get('http://localhost:'+str(PORT)+'/echo?msg=AnyColourYouLike')
        self.assertEqual(res.text, 'AnyColourYouLike', msg='GET echo with a message did not execute successfully')
                                
        #output = subprocess.check_output(['curl', '-s', '-i', '-X', 'GET', 'http://localhost:49160/anUnknownMethod', '|', 'head -n 1'])
        res = requests.get('http://localhost:'+str(PORT)+'/anUnknownMethod')
        self.assertEqual(res.status_code, 404, msg='GET on random unknown resource should have failed, but did not.')
                
        #output = subprocess.check_output(['curl', '-s', '-i', '-X', 'POST', 'http://localhost:49160/hello', '|', 'head -n 1'])
        res = requests.post('http://localhost:'+str(PORT)+'/hello')
        self.assertNotEqual(res.status_code, 200, msg='POST on hello should not have worked, but returned successfully.')
        print (res.status_code)
                                                    
        res = requests.post('http://localhost:'+str(PORT)+'/msg?=post')
        self.assertNotEqual(res.status_code, 200, msg='POST on echo should not have worked, but returned successfully.')

if __name__ == '__main__':
    unittest.main()
