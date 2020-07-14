package se.grama;

/**
 * @author Yo Ka
 */
public class JumpMultiLoop {

    public static void main(String[] args) {
        /*
         * 想跳出多重循环，可以在外面的循环语句前定义一个标号，
         * 然后在里层循环体的代码中使用带有标号的 break 语句，即可跳出外层循环。
         */
        a:
        while (true) {
            while (true) {
                while (true) {
                    break a;
                }
            }
        }

        System.out.println("out");
    }
}
