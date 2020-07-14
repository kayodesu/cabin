package reflect;

/**
 * Status: Pass
 */
public class DeclaringClassTest {
    
    static class A {
        static class B {
            class C {
                
            }
        }
    }

    public static void main(String[] args) {
        System.out.println(DeclaringClassTest.class.getName()); // reflect.DeclaringClassTest
        System.out.println(DeclaringClassTest.class.getDeclaringClass()); // null

        System.out.println(A.class.getName()); // reflect.DeclaringClassTest$A
        System.out.println(A.class.getDeclaringClass()); // class reflect.DeclaringClassTest

        System.out.println(A.B.class.getName()); // reflect.DeclaringClassTest$A$B
        System.out.println(A.B.class.getDeclaringClass()); // class reflect.DeclaringClassTest$A

        System.out.println(A.B.C.class.getName()); // reflect.DeclaringClassTest$A$B$C
        System.out.println(A.B.C.class.getDeclaringClass()); // class reflect.DeclaringClassTest$A$B
    }
    
}
