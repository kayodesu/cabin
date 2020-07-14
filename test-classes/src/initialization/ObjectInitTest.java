package initialization;

/**
 * Status: Pass
 */
public class ObjectInitTest {

    char x;
    int y = 2;
    static double a;
    static double b = 5.7;

    public static void main(String[] args) {
        ObjectInitTest o = new ObjectInitTest();
        
        char t0 = o.x;
        if (t0 == 0) {
            System.out.println("Pass0!");
        } else {
            System.out.println("Fail0!");
        }
        
        int t2 = o.y;
        if (t2 == 2) {
            System.out.println("Pass1!");
        } else {
            System.out.println("Fail1!");
        }

        double t = a;
        if (t == 0) {
           System.out.println("Pass2!");
        } else {
            System.out.println("Fail2!");
        }
        
        double t1 = b;
        if (t1 == 5.7) {
            System.out.println("Pass3!");
        } else {
            System.out.println("Fail3!");
        }
    }

}
