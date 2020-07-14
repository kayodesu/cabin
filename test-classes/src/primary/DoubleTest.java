package primary;

/**
 * Status: Fail
 */
public class DoubleTest {
    private double value;

    public DoubleTest(double value) {
        this.value = value;
    }
    
    public double get() {
        return value;
    }
    
    public static void main(String[] args) {
        DoubleTest tl = new DoubleTest(4349790087343.9483948938493);
        System.out.println(tl.get());
    }
}