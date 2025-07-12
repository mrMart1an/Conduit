namespace namesp {

class TestBase {
public:
    int a;
};

class Child : public TestBase {
public:
    int b;

    int add() const { return a + b;}
};

}
