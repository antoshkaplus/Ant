
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

(1) How to check for tree in construction time.
Probably need a builder for construction.

And gonna be less work for mutator.

The interesting point here is if someone decides to go back and force from 
graph to a tree and vice versa. Do I need to check my graph every time?

(2) Having numbers in class names is not an option.
Only in case of versioning when we leave the old class behind.
Otherwise it's just impossible to get the head around it.

(3) Another thing is how to return new structures from
functions. Probably with time we will get to generalized solution.
But for now can stick to vectors and simple structs.

(4) If in some cases we decide not to have edge descriptors, just values,
there is nothing that would all us access values after certain mutations of the graph.
It's interesting but we may even decide to run without either vertex or edge descriptors.
And it's still fair. How can you do markings then?

(5) how to make random graphs ???

(6) should start with the most generic variant, that supports everything,
after that can think of special specializations.

(7) it looks like only lists can provide with easy Remove/Add. vectors can provide with Guard and Add.
Can do some hybrids but gonna be very confusing.

(8) still in many applications I want to really just have a level on top, describing current graph,
adding/removing certain vertices and edges.

(9) Gonna have to have two kinds of adj list graph: based on lists and based on arrays.
Lists are more versitile while arrays can support stack operations add/remove.
Could be some kind of hybrid of two. 

Even stack operations can go away if we decide to use one big peice of memory for everything.

(10) Derivative graph while wrapper is an expensive one, as all calls has to go through it.
Also very expensive as each time you check if certain step allowed and what additional steps are.

(11) Its important to implement all purpose data strcuture first. That can be extended based on need.
As needs change all the time, and the most general one can be applicable to almost anything.