package interface0;

public class InterfaceDefaultMethodTest {

    class TestInterfaceDefaultTest extends FirstTestClass implements FirstTest, SecondTest { }

    class TestInterfaceFirstTestClass implements FirstTest, SecondTest, ThirdTest, DefaultTest { }

    class FirstTestClass implements ThirdTest, FirstTest { }

    interface FirstTest extends DefaultTest { }

    interface DefaultTest {
        default String test() {
            return "DefaultTest";
        }
    }

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

    public static void main(String[] args) {
        InterfaceDefaultMethodTest test = new InterfaceDefaultMethodTest();
        test.test2();
    }

    //@Test
    public void test1() {
        TestInterfaceDefaultTest testInterfaceDefaultTest = new TestInterfaceDefaultTest();
        DefaultTest defaultTest = testInterfaceDefaultTest;
        FirstTestClass firstTestClass = testInterfaceDefaultTest;
        SecondTest secondTest = testInterfaceDefaultTest;
        ThirdTest thirdTest = testInterfaceDefaultTest;
//        Assert.assertEquals(defaultTest.test(), "ThirdTest");
//        Assert.assertEquals(firstTestClass.test(), "ThirdTest");
//        Assert.assertEquals(secondTest.test(), "ThirdTest");
//        Assert.assertEquals(thirdTest.test(), "ThirdTest");
    }

    //@Test
    public void test2() {
        TestInterfaceFirstTestClass testInterfaceFirstTestClass = new TestInterfaceFirstTestClass();
        DefaultTest defaultTest = testInterfaceFirstTestClass;
        FirstTest firstTest = testInterfaceFirstTestClass;
        SecondTest secondTest = testInterfaceFirstTestClass;
        ThirdTest thirdTest = testInterfaceFirstTestClass;

        System.out.println(defaultTest.test());
        System.out.println(firstTest.test());
        System.out.println(secondTest.test());
        System.out.println(thirdTest.test());
//        Assert.assertEquals(defaultTest.test(), "ThirdTest");
//        Assert.assertEquals(firstTest.test(), "ThirdTest");
//        Assert.assertEquals(secondTest.test(), "ThirdTest");
//        Assert.assertEquals(thirdTest.test(), "ThirdTest");
    }
}
