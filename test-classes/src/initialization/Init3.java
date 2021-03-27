package initialization;

/**
 * Expect output: "init"
 */
public class Init3 {
    private static Init3 o = new Init3();

    private Init3() {
        System.out.println("init");
    }

    public static void main(String[] args) {

    }
}
