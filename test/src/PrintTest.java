/**
 * Status: Fail
 */
public class PrintTest {
    public static void main(String[] args) {
//        System.out.println(false);
//        System.out.println((byte) 3);
//        System.out.println((short) 3);
//        System.out.println('a');
//        System.out.println(123456789);
//        System.out.println(9999999999L);
//        System.out.println(122.445F);
//        System.out.println(122.445);
//        System.out.println(6685785855.285412805887); // 6.685785855285413E9

        String a = "a";
        String b = "b";
        String c = a + b; // 这句在jdk15中竟然生成了 invokedynamic 指令
//        System.out.println(a + b + "c");

//        System.out.printf("%d, %f\n", 435, 959495.64646);
    }
}
