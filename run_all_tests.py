import subprocess


jvm = './cmake-build-release/jvm -cp ./testclasses '


def test_array():
    subprocess.call(jvm + 'array/ArrayTest')
    subprocess.call(jvm + 'array/BubbleSort')


def test_string():
    subprocess.call(jvm + 'string/Mutf8Test')
    subprocess.call(jvm + 'string/StringOut')
    subprocess.call(jvm + 'string/StringTest')


def test_classic():
    """
    some classical problems
    """
    cmd = jvm
    subprocess.call(cmd + 'HelloWorld')


if __name__ == '__main__':
    test_classic()
