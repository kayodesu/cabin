import subprocess

# change the path if necessary
jvm = './cmake-build-release/jvm -cp D:/code/jvm/testclasses '


def test_array():
    subprocess.call(jvm + 'array/ArrayTest')
    subprocess.call(jvm + 'array/BubbleSort')


def test_string():
    subprocess.call(jvm + 'string/Mutf8Test')
    subprocess.call(jvm + 'string/StringOut')
    subprocess.call(jvm + 'string/StringTest')


def test_lambda():
    subprocess.call(jvm + 'lambda/LambdaTest')


def test_exception():
    subprocess.call(jvm + 'exception/UncaughtTest')
    subprocess.call(jvm + 'exception/CatchTest')
    subprocess.call(jvm + 'exception/FinallyTest')


def test_classic():
    """
    some classical problems
    """
    subprocess.call(jvm + 'HelloWorld')


def test_all():
    test_array()
    test_string()
    test_lambda()
    test_exception()
    test_classic()


if __name__ == '__main__':
    # test_all()
    # subprocess.call(jvm + 'exception/StackTraceTest')
    subprocess.call(jvm + 'HelloWorld')
    # subprocess.call(jvm + 'lambda/LambdaTest')
    pass
