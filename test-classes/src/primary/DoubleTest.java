package primary;

/**
 * Status: Fail
 */
public class TestDouble {
    private double value;

    public TestDouble(double value) {
        this.value = value;
    }
    
    public double get() {
        return value;
    }
    
    public static void main(String[] args) {
        TestDouble tl = new TestDouble(4349790087343.9483948938493);
        System.out.println(tl.get());
    }
}