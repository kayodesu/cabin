
/**
 * Status: Pass
 */
public class TryCatchFinallyTest {

    int func1() {
        int i = 1;
        try {
            return i;
        } finally {
            i = 2;
        }
    }

    int func2() {
        int i = 1;
        try {
            return i;
        } finally {
            i = 2;
            return 2;
        }
    }

    void test1() {
        System.out.println(func1() == 1);
    }

    void test2() {
        System.out.println(func2() == 2);
    }

    public static void main(String[] args) {
        TryCatchFinallyTest o = new TryCatchFinallyTest();
        o.test1();
        o.test2();
    }
}
