package wrapper;

/**
 * Status: Pass
 */
public class FloatTest {
    public static void main(String[] args) {
        System.out.println(1076754509 == Float.floatToRawIntBits(2.71828f) ? "Pass" : "Fail");
        System.out.println(2.71828f == Float.intBitsToFloat(1076754509) ? "Pass" : "Fail");
    }
}
