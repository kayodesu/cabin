package lambda;

import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;
import java.util.function.Consumer;

/**
 * Status: Fail
 */
public class LambdaTest1 {
    int j = 100;
    public void test1(Integer i){
        System.out.println("test1");
        System.out.println(this.j);
        System.out.println(i);
    }
    public void testLambda(){
        System.out.println("labda begin");
        Consumer<Integer> r1 = this::test1;
//        r1.accept(43);
//        MethodType mt= MethodType.methodType(String.class,int.class);
//        try{
//            MethodHandle mh = MethodHandles.lookup().findVirtual(String.class, "substring", mt);
//            System.out.println("before concat");
//            //System.out.println(mh);
//            String result=(String)
//                    mh.invokeExact("STRING", 1);
//            System.out.println(result);
//        }
//        catch(Throwable e){
//            throw new RuntimeException(e);
//        }
    }

    public static void main(String[] args) {
        new LambdaTest1().testLambda();
    }
}
