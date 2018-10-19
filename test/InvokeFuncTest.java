
public class InvokeFuncTest implements Runnable {
    public static void main(String[] args) {
        new InvokeFuncTest().test();
    }

    public void test() {
        InvokeFuncTest.staticMethod(); // invokestatic
        InvokeFuncTest demo = new InvokeFuncTest(); // invokespecial
        demo.instanceMethod(); // invokespecial
        super.equals(null); // invokespecial
        this.run(); // invokevirtual
        ((Runnable) demo).run(); // invokeinterface
    }

    public static void staticMethod() {
    }

    private void instanceMethod() {
    }

    @Override
    public void run() {
    }
}
