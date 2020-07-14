package exception;

/**
 * Status: Fail
 */
public class InstructionNpeTest {
    
    private int i;

    public static void main(String[] args) throws Exception {
        arraylength();
        athrow();
        getfield();
        monitorenter();
        invokevirtual();
    }

    public static void arraylength() {
        try {
            int[] x = null;
            int y = x.length;
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }

    public static void athrow() throws Exception {
        try {
            Exception x = null;
            throw x;
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }

    public static void getfield() {
        try {
            InstructionNpeTest x = null;
            int y = x.i;
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }

    public static void monitorenter() {
        try {
            Object x = null;
            synchronized(x) {
                System.out.println("BAD!");
            }
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }

    public static void invokevirtual() {
        try {
            Object x = null;
            x.toString();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }
    
}
