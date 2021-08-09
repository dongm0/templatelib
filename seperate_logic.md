# 提取表面的逻辑

## EnumerateAllSFcase()

主要的method，**步骤**：
- 提取表面
- 提取对角线（向上的forbidden）
- 对所有的面i枚举（i作为中心），合理的情况加入res里面

## SFCaceAt

输入：cell，以及表面四边形在cell中的位置  
输出：所有合法的情况  
**步骤**：
- 查询四个相邻面的连接情况（可以初步筛选掉一些情况）
- 检查每一种情况（checkSFCase）

## checkSFCase

输入参数：iter_time 循环次数，sfcase 表面种类，corn 每个循环里检查角的位置，corn_val 对应的值（对应相同就不计算），faces 相邻面（循环i==0时的位置，不计算中心）， diag_tests 要检查对角线的两端点（长为4，[0]和[2]是两端所在的面编号（中心是0，逆时针计算，不空）（与faces中的编号不同，虽然看起来麻烦但是实际写逻辑的时候方便所以就这样了），[1]和[3]是在面内的编号）

**例**  
```
*---------*
|3       2|   faces:0
|0       1|   diag_tests中编号：1 
*---------*
|1       0|   faces:无
|2       3|   diag_tests中编号：0  
*---------*  
|1       0|   faces:2
|2       3|   diag_tests中编号：2  
*---------*  
```
- iter_time:2
- sface:SFCase::F3_1
- corn:{0, 1, 2, 3}
- corn_val:{true, true, true, true}
- faces:{0, 2}
- diag_tests:{{0, 2, 1, 2}, {0, 3, 1, 3}, {0, 0, 2, 2}, {0, 1, 2, 3}, {1, 2, 2, 2}, {1, 3, 2, 3}}（其实还有侧面的对角线要判断，但是已经被corn_val判断过了）