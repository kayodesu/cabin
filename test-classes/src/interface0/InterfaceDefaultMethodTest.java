package interface0;

/**
 * Status: Pass
 */
public class InterfaceDefaultMethodTest {
    interface DefaultTest {
        default String test() {
            return "DefaultTest";
        }
    }
    
    interface FirstTest extends DefaultTest { }

    interface SecondTest extends FirstTest {
        default String test() {
            return "SecondTest";
        }
    }

    interface ThirdTest extends DefaultTest, SecondTest {
        default String test() {
            return "ThirdTest";
        }
    }

    class FirstTestClass implements ThirdTest, FirstTest { }
    
    class TestInterfaceDefaultTest extends FirstTestClass implements FirstTest, SecondTest { }

    class TestInterfaceFirstTestClass implements FirstTest, SecondTest, ThirdTest, DefaultTest { }

    public static void main(String[] args) {
        InterfaceDefaultMethodTest test = new InterfaceDefaultMethodTest();
        test.test1();
        test.test2();
    }

    public void test1() {
        TestInterfaceDefaultTest testInterfaceDefaultTest = new TestInterfaceDefaultTest();
        DefaultTest defaultTest = testInterfaceDefaultTest;
        FirstTestClass firstTestClass = testInterfaceDefaultTest;
        SecondTest secondTest = testInterfaceDefaultTest;
        ThirdTest thirdTest = testInterfaceDefaultTest;

        System.out.println(defaultTest.test() == "ThirdTest");
        System.out.println(firstTestClass.test() == "ThirdTest");
        System.out.println(secondTest.test() == "ThirdTest");
        System.out.println(thirdTest.test() == "ThirdTest");
    }

    public void test2() {
        TestInterfaceFirstTestClass testInterfaceFirstTestClass = new TestInterfaceFirstTestClass();
        DefaultTest defaultTest = testInterfaceFirstTestClass;
        FirstTest firstTest = testInterfaceFirstTestClass;
        SecondTest secondTest = testInterfaceFirstTestClass;
        ThirdTest thirdTest = testInterfaceFirstTestClass;

        System.out.println(defaultTest.test() == "ThirdTest");
        System.out.println(firstTest.test() == "ThirdTest");
        System.out.println(secondTest.test() == "ThirdTest");
        System.out.println(thirdTest.test() == "ThirdTest");
    }
}
