package object;

/**
 * Status: Pass
 */
public class InheritanceTest {
    
    public static void main(String[] args) {
        Sub sub = new Sub();
        sub.x = 1;
        sub.y = 2L;
        sub.a = 3.14f;
        sub.b = 2.71828;
        
        int x = sub.x;
        long y = sub.y;
        float a = sub.a;
        double b = sub.b;
        
        Super sup = sub;
        long j = sup.x + sup.y;
        
        System.out.println((x == 1 && y == 2 && a == 3.14f && b == 2.71828 && j == 3L) ? "Pass":"Fail");
    }
    
}

class Super {
    int x;
    long y;
}

class Sub extends Super {
    float a;
    double b;
}
