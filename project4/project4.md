# Project 4

## Race Conditions

#### 1. 
>Expected result if 3 threads run it and `x` was initialized to `12:13` How could it be different?
```
1│ if x == 12:
2│     x++
```
##### :warning: What can go wrong:
1. T1 can check if `x` is `12` and finds it true.
2. T2 can check if `x` is `12` and finds it true.
3. T1 can increment `x` to `13`. 
4. T2 can increment `x` to `14`.

##### :bulb: Solution:
Change the if to a while to check that `x` is `12` and place locks around the incrementor. 

```
1| while x == 12:
2|      lock(m1)
3|      x++
4|      unlock(m1)
```

#### 2.
>Expected result if 3 threads run ti and `x` was initialized to `12`:`13`. How could it be different?

```
1| if x == 12:
2|      lock(m1)
3|      x++
4|      unlock(m1)
```

##### :warning: What can go wrong:
1. T1 can check if `x` is `12` and finds it true.
2. T2 can check if `x` is `12` and finds it true.
3. T1 locks `m1` and increments `x` to `13`.
4. T1 unlocks `m1`. 
5. T2 locks `m1` and increments `x` to `14`. 
6. T2 unlocks `m1`. 

##### :bulb: Solution:
Change the if to a while to check that `x` is `12`

```
1| while x == 12:
2|      lock(m1)
3|      x++
4|      unlock(m1)
```

#### 3. In this problem, assume the hash itself is already threadsafe.
>Expected result if 3 threads run it and `y` was not in the hash to start: `14`. How could it be different?

```
1│ if y not in hash:
2│     hash[y] = 12
3│ else
4│     hash[y]++
```
##### :warning: What could go wrong:
1. T1 checks if `y` is in the hash, it is not.
2. T2 checks if `y` is in the hash, it is not. 
3. T1 sets `hash[y]` to `12`
4. T2 sets `hash[y]` to `12`
5. T3 checks if `y` is in the hash, it is
6. T3 increments `hash[y]` to `13`

##### :bulb: Solution:
Change the if check to a while, and add locks around the hash

```
1│ while y not in hash:
2|      lock(m1)
3│      hash[y] = 12
4|      unlock(m1)
5│ else
6|      lock(m1)
7│      hash[y]++
8|      unlock(m1)
```
#### 4. 
>Expected result if 3 threads run it and `x` was initialized to `0`:`36`. How could it be different?

```
x += 12
```

##### :warning: What could go wrong:
1. T1 reads the value of `x` to `0`
2. T1 is interrupted
3. T2 reads the value of `x` to `0`
4. T1 continues and calculates `x + 12`
5. T2 continues and calculates `x + 12`
6. T1 stores the calculated value, `12` to `x`
7. T2 overwrites T1 change to `x` storing its calculated value, `12` to `x`

##### :bulb: Solution: 
Add locks around the line of code. 
```
lock(m1)
x += 12
unlock(m1)
```

#### 5. This is an implementation of a semaphore meant to be used by other code. 

>`semaphore_init()` is documented as being not-threadsafe, so you don't have to worry about any races in that particular function.
>
>Assuming just one thread called `semaphore_init(1)`, then no matter how many threads call the signal and wait functions, `x` should never fall below `0`. How could that be violated?
>
>:exclamation: Hint: this one is a bit tricky. What other tools in addition to mutexes have we learned about?

```
 1│ semaphore_init(value):
 2│     x = value
 3│
 4│ semaphore_signal():
 5│     x++
 6│
 7│ semaphore_wait():
 8│     while x == 0:
 9│         do nothing  # spinlock
10│
11│     x--
```

##### :warning: What could go wrong:
The issue is in the `semaphore_wait()`. If multiple threads call this function when `x` is `0`, each thread would enter the spin lock and continue waiting forever using CPU cycles. Alternatively, if one thread calls `semaphore_signal()` and increments `x` to `1`. Other threads might not immediately see the updated value and continue consuming CPU resources. 

##### :bulb: Solution:
To fix these issues we can replace the spinlock with a condition variable and locks to accomplish some control. 

```
 1│ semaphore_init(value):
 2│     x = value
 3│
 4│ semaphore_signal():
 5|     lock(m1)
 6│     x++
 7|     signal(cv)
 8|     unlock(m1)
 9│
10│ semaphore_wait():
11|     lock(m1)
12│     while (x == 0) {
13│         wait(cv, m1);
14|     }
15│     x--
16|     unlock(m1)
```

## Deadlocks

#### 1. "Out of Order

>Find a solution without removing the annoying code on lines 12-13, and without adding any more locks or unlocks. 

```
 1│ function1():
 2│     lock(m1)
 3│     lock(m2)
 4│
 5│     unlock(m2)
 6│     unlock(m1)
 7│
 8│ function2():
 9│     lock(m1)
10│     lock(m2)
11│
12│     unlock(m1)
13│     lock(m1)
14│
15│     unlock(m2)
16│     unlock(m1)
```
##### :warning: What could go wrong:
1. T1 acquires `m1` in `function1` (line 2)
2. T2 acquires `m2 in  `function2` (line 10)
3. T1 tries to acquire `m2` but is blocked
4. T2 tries to acquire `m1` again (line 13) but is blocked because T1 already holds it. 

##### :bulb: Solution
To modify this while keeping lines 12-13 we can move the `unlock(m2)` to line 11. This releases the lock and removes the deadlock. 

```
 1│ function1():
 2│     lock(m1)
 3│     lock(m2)
 4│
 5│     unlock(m2)
 6│     unlock(m1)
 7│
 8│ function2():
 9│     lock(m1)
10│     lock(m2)
11│
12│     unlock(m2) // move
13│     unlock(m1)
14│     lock(m1)
15│
16│     unlock(m1)
```

#### 2. Twisting little passages, all different...

>In the following, assume you ___cannot___ make the mutexes `m1` or `m2` global or require the callers to pass them in any exact order.

```
1│ function1(m1, m2):  # Mutexes are passed in by caller
2│     lock(m1)
3│     lock(m2)
4│
5│     unlock(m2)
6│     unlock(m1)
```
##### :warning: What could go wrong:
The deadlock issue comes up if one thread passes in `m1` and `m2` and another thread passes in `m2` and `m1`. The opposite order. Neither thread can proceed as they are waiting for the other to release the lock it needs. 

##### :bulb: Solution
One solution to this is to use a third lock. This lock is acquired in the beginning of the function. This eliminates the need for making either `m1` or `m2` global or requiring users to pass them in an exact order.

```
1│ function1(m1, m2):  # Mutexes are passed in by caller
2|      lock(m3)
3│      lock(m1)
4│      lock(m2)
5│
6│      unlock(m2)
7│      unlock(m1)
8|      unlock(m3)
```
