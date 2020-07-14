package wrapper;

/**
 * Status: Pass
 */
public class DoubleTest {
    public static void main(String[] args) {
        System.out.println(4614253070214989087L == Double.doubleToRawLongBits(3.14) ? "Pass" : "Fail");
        System.out.println(3.14 == Double.longBitsToDouble(4614253070214989087L) ? "Pass" : "Fail");
    }
}
