import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Arrays;

public class Utils {
    public static void separator(String title) {
        System.out.println("------ " + title + " ------");
    }

    @Target(ElementType.METHOD)
    @Retention(RetentionPolicy.RUNTIME)
    public @interface TestMethod {
        boolean pass();
    }

    public static void invokeAllTestMethods(Class<?> c) {
        Method[] methods = c.getDeclaredMethods();
        for (Method m : methods) {
            var an = m.getDeclaredAnnotation(Utils.TestMethod.class);
            if (an != null) {
                separator(m.getName());
                try {
                    m.invoke(null);
                } catch (IllegalAccessException | InvocationTargetException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
