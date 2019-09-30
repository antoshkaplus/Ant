
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

* Always check newly opened file stream for validity with .fail() call, should be also 
used during read to prevent reading invalid values that can cause undefined execution

### optional
std::optional is only for value type and is not a replacement for std::pair<Value, bool>
where it's not known if result gonna be returned due to std::optional can't take references.
And references are returned very frequently in OOP. But std::optional can probably find itself 
as member variable where it's not known if member is present or not.