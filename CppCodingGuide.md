
### Style
* Files and directories are low cased, use underscore as a delimiter
* Class names are Capital Cased
* Method names are capital cased, excluding getters and setters. Getter name corresponds to it's name 
to avoid naming collusion member names have underscore at the end.

### Be Aware
* Undefined order of evaluation:
```c++
std::unordered_map<int, int> m;
auto it = m.insert(val) - m.begin();
```
"begin" can be evaluated first and if "m" grows in size on insert "it" value is invalid