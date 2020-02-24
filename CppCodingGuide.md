
### Style
* Files and directories are low cased, use underscore as a delimiter
* Class names are Capital Cased
* Method names are capital cased, excluding getters and setters. Getter name corresponds to it's name 
to avoid naming collusion member names have underscore at the end.
* Class number in name can't describe "another implementation" as it's impossible to make sense of.
Number can used only be in versioning, so that older version there is just for compatility reasons and should 
not be used, be avoided.

### Be Aware
* Undefined order of evaluation:
```c++
std::unordered_map<int, int> m;
auto it = m.insert(val) - m.begin();
```
"begin" can be evaluated first and if "m" grows in size on insert "it" value is invalid

* Always check newly opened file stream for validity with .fail() call, should be also 
used during read to prevent reading invalid values that can cause undefined execution
* Start customizable types with implementation of variant that supports as many most important 
features as possible / most common variant and only after that focus on specializations.
 
### running tests:
* make sure working directory is a subdirectort of the project  
* --gtest_filter=*  --gtest_break_on_failure

### optional
std::optional is only for value type and is not a replacement for std::pair<Value, bool>
where it's not known if result gonna be returned due to std::optional can't take references.
And references are returned very frequently in OOP. But std::optional can probably find itself 
as member variable where it's not known if member is present or not.

### functions vs methods
While methods provide: polymorphism, incapsulation and inheritance, what about using functions?
Sometimes you may find that two "containers" have different structure but certain algorithms can 
be used for both of them at the same time. In that case OOP would offer inheritance. But with
function template you could probably reuse much easier. So now we have this function that is out of both
classes. In some united file with unknown name and we probably would have to make this function friend for
both classes. And now we have two different methodologies of code structure.

It looks like that for testing there is always has to be some "base" version of the data structure / complex class.
This "base" open as much as possible implementation for easy testing. Some things that work for different structs could 
be put in common function. After that we use Facade over it for common user that closes all the nitty-gritty parts
leaving only nice interface for usage.

Now question arises how to namespace everything. "base" namespace should surely follow only "data type name" namespace.
And "data type namespace" needed to get out of common namespace certain function names and structures that could be found in 
other places. Can it be done without the base class? - If scope is small developer can have main Facade class and Base class 
under the same main namespace where Base class opens up more things for testing and then just used through composition in
Facade class. In case where we have inner classes, many similar data types of the same scope 
it's best to have everything under one scope namespace and then base namespace for testing implementations. 
Also developer can easily inject main types into main namespace with "using" directive.

Small scope - use classes to separate, Bigger scope (more than one class) - have to use namespaces. 

If functions are needed to be templated it's better to use class of static methods to reuse template parameters.

### static classes
While static classes are great for declaration of template parameter scope, inner classes inside won't
get template parameter deduction for functions outside. Due to deduction happens only with parameters 
after last ::. So certain helper classes should be left outside of static class with it's functions.

If namespacing too much we lose ability to use overloaded functions in template functions.
So keep some common function in the base class, move other details to nested namespaces.

So keeping certain type of functionality in separate namespaces should help. Also that way you also write your tests.
So it's a little bit different than namespacing on type of data structure. It is in ortogonal space.

####

While doing this

template <typename B>
class A {
    using SizeType = B
}

we don't postfix -Type on parameter below, because that's name of the type

template <typename Size>
class A {}

using SizeType = B - naming Type of curtain property of the class. 
Size here is a property name, not the type name.

#### 
