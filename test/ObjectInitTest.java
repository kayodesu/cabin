
public class ObjectInitTest {

    char x;
    static long a;

    public static void main(String[] args) {
        ObjectInitTest o = new ObjectInitTest();
        char y = o.x;
        if (y == 0) {
            System.out.println("OK!");
        } else {
            System.out.println("Fail1!");
        }

        long b = a;
        if (b == 0) {
            System.out.println("OK2!");
        } else {
            System.out.println("Fail2!");
        }
    }

}
