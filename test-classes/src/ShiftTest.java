/**
 * Status: Pass
 */
public class ShiftTest {

    public static void main(String[] args) {
        // <<: 左移运算符，num << 1,相当于num乘以2
        // >>：带符号右移。正数右移高位补0，负数右移高位补1。
        // >>>：无符号右移。无论是正数还是负数，高位通通补0。
        // 对于正数而言，>>和>>>没区别。

        System.out.println((-2 << 1 == -4) ? "Pass" : "Fail");
        System.out.println((Integer.MAX_VALUE << 1 == -2) ? "Pass" : "Fail");

        System.out.println((4 >> 1 == 2) ? "Pass" : "Fail");
        System.out.println((-4 >> 1 == -2) ? "Pass" : "Fail");
        System.out.println((-4 >>> 1 == Integer.MAX_VALUE - 1) ? "Pass" : "Fail");

        System.out.println((2 >> 1 == 1) ? "Pass" : "Fail");
        System.out.println((-2 >> 1 == -1) ? "Pass" : "Fail");
        System.out.println((-2 >>> 1 == Integer.MAX_VALUE) ? "Pass" : "Fail");

        System.out.println((1 >> 1 == 0) ? "Pass" : "Fail");
        System.out.println((-1 >> 1 == -1) ? "Pass" : "Fail");
        System.out.println((-1 >>> 1 == Integer.MAX_VALUE) ? "Pass" : "Fail");

    }
}
