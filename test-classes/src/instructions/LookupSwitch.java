package instructions;

/**
 * Status: Fail
 */
public class LookupSwitch {
    private static void test(int i) {
        switch (i) {
            case -100: System.out.println("-100"); break;
            case 0: System.out.println("0"); break;
            case 3: System.out.println("3"); break;
            case 5: System.out.println("5"); break;
            default: System.out.println("default");
        }
    }
    
    public static void main(String[] args) {
        test(-100);

        for (int i = 1; i < 7; i++)
            test(i);
    }
}
