# 🎯 Udacity C++ Nanodegree Project Rubric

Use this project rubric to understand and assess the project criteria.

------------------------------------------------------------------------

## 🧾 README (All Rubric Points REQUIRED)

  -----------------------------------------------------------------------
  **Criteria**           **Submission Requirements**
  ---------------------- ------------------------------------------------
  **A README with        The README file must include instructions for
  instructions is        **building** and **running** the project.
  included with the      
  project**              

  -----------------------------------------------------------------------

If any additional libraries are required, list them with
**cross-platform installation instructions**.\
You may submit your write-up as **Markdown (.md)** or **PDF (.pdf)**. \|
\| **The README indicates the new features you added to the game** \|
The README must clearly list the **new features** you added to the game,
along with the **expected behavior** or **output** of each feature. \|
\| **The README includes information about each rubric point addressed**
\| The README must specify **which rubric points** are addressed and
**where** they are implemented in the code (e.g. file names and line
numbers). \|

------------------------------------------------------------------------

## 🧱 Compiling and Testing (All Rubric Points REQUIRED)

  -----------------------------------------------------------------------
  **Criteria**           **Submission Requirements**
  ---------------------- ------------------------------------------------
  **The submission must  The project must **build and run successfully**
  compile and run        in the **Udacity workspace**.
  without errors on the  
  Udacity project        
  workspace**            

  -----------------------------------------------------------------------

It is **strongly recommended** to use **CMake** and **Make**, as
provided in the starter repository.\
If you use another build system, ensure that it **compiles
successfully** in the Udacity workspace. \|

------------------------------------------------------------------------

## 🔁 Loops, Functions, and I/O (Meet at least 2 criteria)

  -----------------------------------------------------------------------
  **Criteria**           **Submission Requirements**
  ---------------------- ------------------------------------------------
  **The project          A variety of **control structures** (e.g. loops,
  demonstrates an        conditionals) are implemented.
  understanding of C++   
  functions and control  
  structures**           

  The project code is    
  **clearly organized**  
  into functions.        

  **The project reads or The project **reads data** from an **external
  writes data from/to a  file** or **writes data** to a file as part of
  file**                 its core operation.

  **The project accepts  In addition to controlling the snake, the game
  and processes user     can accept and process **new types of player
  input**                input**.

  **The project uses     The project uses **arrays** or **vectors**, and
  data structures and    makes use of **constant variables (`const`)**
  immutable variables**  where appropriate.
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## 🧩 Object-Oriented Programming (Meet at least 3 criteria)

  -----------------------------------------------------------------------
  **Criteria**           **Submission Requirements**
  ---------------------- ------------------------------------------------
  **One or more classes  Classes are designed with **attributes** (data)
  are added to the       and **methods** (behavior).
  project with           
  appropriate access     
  specifiers**           

  -----------------------------------------------------------------------

All data members are explicitly marked as **public**, **protected**, or
**private**.\
Data subject to invariants is **hidden** and accessed via
**getter/setter methods**. \| \| **Class constructors utilize member
initialization lists** \| All class members that are initialized from
constructor arguments must use **member initialization lists**. \| \|
**Classes abstract implementation details from their interfaces** \|
Each class hides implementation details and exposes only necessary
interfaces.\
Member functions document their effects (via clear names, comments, or
documentation).\
No function should change program state in undocumented ways. \| \|
**Overloaded functions allow the same function to operate on different
parameters** \| At least one function is **overloaded** with different
**signatures** under the same name. \| \| **Classes follow an
appropriate inheritance hierarchy with virtual and override functions**
\| Inheritance hierarchies are **logical**.\
At least one **derived class** overrides a **virtual function** from its
**base class**. \| \| **Templates generalize functions or classes** \|
At least one **function** or **class** uses **templates** to accept
**generic parameters**. \|

------------------------------------------------------------------------

## 🧠 Memory Management (Meet at least 3 criteria)

  -----------------------------------------------------------------------
  **Criteria**           **Submission Requirements**
  ---------------------- ------------------------------------------------
  **The project makes    At least two variables are defined as
  use of references in   **references**, or at least two functions use
  function               **pass-by-reference**.
  declarations**         

  **The project uses     Classes that use **unmanaged dynamically
  destructors            allocated memory** (or require cleanup) must
  appropriately**        define an appropriate **destructor**.

  **The project uses     The project follows **Resource Acquisition Is
  scope / RAII where     Initialization (RAII)** principles:
  appropriate**          
  -----------------------------------------------------------------------

-   Allocate objects at compile-time\
-   Initialize objects at declaration\
-   Rely on scope for **automatic destruction** \| \| **The project
    follows the Rule of Five** \| For any class that defines one of the
    following: **copy constructor**, **copy assignment operator**,
    **move constructor**, **move assignment operator**, or
    **destructor**, it must define **all five**. \| \| **The project
    uses move semantics instead of copying where possible** \| The
    project must prefer **move semantics** (using `std::move`) to avoid
    unnecessary copying. \| \| **The project uses smart pointers instead
    of raw pointers** \| The project must use at least one **smart
    pointer**: `std::unique_ptr`, `std::shared_ptr`, or `std::weak_ptr`.
    \|

------------------------------------------------------------------------

## ⚙️ Concurrency (Meet at least 2 criteria)

  -----------------------------------------------------------------------
  **Criteria**           **Submission Requirements**
  ---------------------- ------------------------------------------------
  **The project uses     The project must use **multiple threads** or
  multithreading**       **asynchronous tasks** (`std::async`).

  **A promise and future A **`std::promise`** and **`std::future`** are
  is used in the         used to pass data from a **worker thread** to a
  project**              **parent thread**.

  **A mutex or lock is   A **mutex** or **lock** (e.g. `std::lock_guard`,
  used in the project**  `std::unique_lock`) is used to **protect shared
                         data** across threads.

  **A condition variable A **`std::condition_variable`** is used to
  is used in the         **synchronize thread execution**.
  project**              
  -----------------------------------------------------------------------
