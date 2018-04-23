package io.github.jiayanggo.gson;


public class GsonTest {

    public static void main(String[] args) {
        GsonTest test = new GsonTest();
        test.gson();
        test.testGenericsType();
    }

    public void gson() {
        JsonTest2 obj = new JsonTest2();
        obj.x = 100;
        obj.y = 3.14f;
//        Assert.assertEquals("{\"x\":100,\"y\":3.14}", new Gson().toJson(obj));
    }

    public void testGenericsType() {
        try {
            JsonTest<String> jsonTest = new JsonTest<String>();
            jsonTest.x = "beyond";
//            Assert.assertEquals("{\"x\":\"beyond\"}", new Gson().toJson(jsonTest));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

class JsonTest<T> {
    public T x;
}

class JsonTest2 {
    public int x;
    public float y;
}
