import unittest
import os


# class MyTestCase(unittest.TestCase):
#     def test_something(self):
#         self.assertEqual(True, False)

javac = os.environ['JAVA_HOME'] + '\\bin\\javac.exe '
kayovm = '.\\cmake-build-debug\\kayovm.exe -cp "xxxxxxx" '  # 指定用 debug 还是 release


def run_test(class_name):
    java_file = ".\\testclasses\\" + class_name + ".java"

    print(javac)
    print(java_file)
    print(kayovm)

    os.startfile(javac, java_file)
    os.startfile(kayovm, class_name)


if __name__ == '__main__':
    run_test('HelloWorld')
    
    run_test('exception/StackTraceTest')
    
    run_test('invoke/MethodHandleTest')
    
    run_test('lambda/LambdaTest')
    #   unittest.main()
