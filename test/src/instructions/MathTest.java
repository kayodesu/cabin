package instructions;

/**
 * Status: Pass
 */
public class MathTest {

    private static void testShift() {
        System.out.println(-2 << 1); // -4
        System.out.println(Integer.MAX_VALUE << 1); // -2

        System.out.println(4 >> 1); // 2
        System.out.println(-4 >> 1); // -2
        System.out.println(-4 >>> 1); // 2147483646（Integer.MAX_VALUE - 1）

        System.out.println(2 >> 1); // 1
        System.out.println(-2 >> 1); // -1
        System.out.println(-2 >>> 1); // 2147483647（Integer.MAX_VALUE）

        System.out.println(1 >> 1); // 0
        System.out.println(-1 >> 1); // -1
        System.out.println(-1 >>> 1); // 2147483647（Integer.MAX_VALUE）

        System.out.println(12345 >> 13); // 1
        System.out.println(-12345 >> 13); // -2
        System.out.println(-12345 >>> 13); // 524286

        long v = 12345678987654321L;
        System.out.println(v >> 13); // 1507040892047
        System.out.println(-v >> 13); // -1507040892048
        System.out.println(-v >>> 13); // 2250292772793200
    }

    private static void testRem() {
        int i = 3;
        int j = 2;
        System.out.println(i%j); // irem 1

        float f1 = 23.56f;
        float f2 = 11.555f;
        System.out.println(f1%f2); // frem 0.44999886

        double d1 = 23444555454.554667776;
        double d2 = 4353511.555699655;
        System.out.println(d1%d2 == 895727.1120270332); // drem 895727.1120270332
        System.out.println(d1%d2); // drem 895727.1120270332
    }

    private static void testLogic() {
        long i = -776565656565677519L;
        long j = 457688856546456422L;
        System.out.println(i & j); // 294987450898580000
        System.out.println(i | j); // -613864250917801097
        System.out.println(i ^ j); // -908851701816381097
    }

    private static void testIinc() {
        int i = 50;

        i += -100; // iinc
        System.out.println(i);

        i += 1000; // wind iinc
        System.out.println(i);
    }

    public static void main(String[] args) {
        MathTest.testShift();
        MathTest.testRem();
        MathTest.testLogic();
        MathTest.testIinc();
    }
}
