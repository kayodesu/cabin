
public class WrapperTest {

    @Utils.TestMethod(pass = false)
    public static void testDouble() {
        System.out.println(4614253070214989087L == Double.doubleToRawLongBits(3.14) ? "Pass" : "Fail");
        System.out.println(3.14 == Double.longBitsToDouble(4614253070214989087L) ? "Pass" : "Fail");
    }

    @Utils.TestMethod(pass = false)
    public static void testFloat() {
        System.out.println(1076754509 == Float.floatToRawIntBits(2.71828f) ? "Pass" : "Fail");
        System.out.println(2.71828f == Float.intBitsToFloat(1076754509) ? "Pass" : "Fail");
    }

    @Utils.TestMethod(pass = false)
    public static void integerCache() {
        Integer a = -128;
        Integer b = -128;
        System.out.println(a == b ? "Pass" : "Fail");

        Integer a1 = 0;
        Integer b1 = 0;
        System.out.println(a1 == b1 ? "Pass" : "Fail");

        Integer a2 = 127;
        Integer b2 = 127;
        System.out.println(a2 == b2 ? "Pass" : "Fail");

        Integer c = -129;
        Integer d = -129;
        System.out.println(c != d ? "Pass" : "Fail");

        Integer c1 = 128;
        Integer d1 = 128;
        System.out.println(c1 != d1 ? "Pass" : "Fail");

        System.out.println(new Integer(1) != new Integer(1) ? "Pass" : "Fail");
    }

    public static void main(String[] args) {
        Utils.invokeAllTestMethods(WrapperTest.class);
    }
}
