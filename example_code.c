// 时间旅行调试器示例程序
// 本程序展示了时间旅行调试器支持的所有语法特性

// 注意：本解释器支持极简C语言子集，不支持函数、指针、结构体等高级特性

int main() {
    // ========== 1. 变量声明和初始化 ==========
    // 基本类型变量
    int x = 10;
    float y = 3.14159;
    bool flag = true;

    // 数组声明和初始化
    int numbers[5];
    numbers[0] = 1;
    numbers[1] = 2;
    numbers[2] = 3;
    numbers[3] = 4;
    numbers[4] = 5;

    float temperatures[3];
    temperatures[0] = 36.5;
    temperatures[1] = 37.2;
    temperatures[2] = 35.8;

    // ========== 2. 算术运算 ==========
    int sum = x + 5;          // 15
    float product = y * 2.0;  // 6.28318
    int difference = 20 - x;  // 10
    float quotient = y / 2.0; // 1.570795

    // 混合类型运算（int和float）
    float mixed = x + y;      // 13.14159

    // ========== 3. 比较运算 ==========
    bool isGreater = x > 5;        // true
    bool isEqual = x == 10;        // true
    bool isNotEqual = x != 15;     // true
    bool isLessOrEqual = x <= 10;  // true

    // ========== 4. 逻辑运算 ==========
    bool logicalAnd = (x > 5) && (x < 15);  // true
    bool logicalOr = (x < 5) || (x > 8);    // true
    bool logicalNot = !flag;                // false

    // ========== 5. 条件语句 ==========
    print("=== 条件语句示例 ===");

    if (x > 5) {
        print("x is greater than 5");
    } else {
        print("x is 5 or less");
    }

    // 嵌套if-else
    if (x > 15) {
        print("x is greater than 15");
    } else if (x > 10) {
        print("x is greater than 10 but not greater than 15");
    } else {
        print("x is 10 or less");
    }

    // ========== 6. 循环语句 ==========
    print("\n=== 循环语句示例 ===");

    // while循环
    int i = 0;
    while (i < 3) {
        print("While loop iteration: ");
        print(i);
        i = i + 1;
    }

    // 循环中的条件判断
    int j = 0;
    while (j < 5) {
        if (j == 2) {
            print("Found j == 2");
        }
        j = j + 1;
    }

    // ========== 7. 数组操作 ==========
    print("\n=== 数组操作示例 ===");

    // 数组元素访问
    print("First number: ");
    print(numbers[0]);

    print("Second temperature: ");
    print(temperatures[1]);

    // 数组遍历
    int k = 0;
    while (k < 5) {
        print("numbers[");
        print(k);
        print("] = ");
        print(numbers[k]);
        k = k + 1;
    }

    // ========== 8. 复杂表达式 ==========
    print("\n=== 复杂表达式示例 ===");

    // 算术表达式
    int complexExpr = (x + 5) * 2 - 3;  // (10+5)*2-3 = 27
    print("(x + 5) * 2 - 3 = ");
    print(complexExpr);

    // 逻辑表达式
    bool complexLogic = (x > 5) && (x < 20) || (flag == true);
    print("(x > 5) && (x < 20) || (flag == true) = ");
    print(complexLogic);

    // ========== 9. 变量重新赋值 ==========
    print("\n=== 变量重新赋值示例 ===");

    x = 20;
    print("New x value: ");
    print(x);

    flag = false;
    print("New flag value: ");
    print(flag);

    // 数组元素重新赋值
    numbers[2] = 100;
    print("New numbers[2] value: ");
    print(numbers[2]);

    // ========== 10. 最终状态输出 ==========
    print("\n=== 最终变量状态 ===");

    print("x = ");
    print(x);

    print("y = ");
    print(y);

    print("flag = ");
    print(flag);

    print("sum = ");
    print(sum);

    print("product = ");
    print(product);

    // 最终数组状态
    int m = 0;
    while (m < 5) {
        print("Final numbers[");
        print(m);
        print("] = ");
        print(numbers[m]);
        m = m + 1;
    }

    print("\n=== 程序执行完成 ===");

    return 0;
}

// 使用说明：
// 1. 将此代码复制到时间旅行调试器的编辑器中
// 2. 点击"运行"按钮执行整个程序
// 3. 点击"单步"按钮逐条语句执行
// 4. 使用底部滑块进行时间旅行，查看不同步骤的变量状态
// 5. 观察右上角的变量监控表格，查看变量值的变化
// 6. 观察右下角的控制台输出，查看程序输出结果