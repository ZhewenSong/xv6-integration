# xv6

<p>
This is an implementation of MLFQ scheduler, virtual memory layout rearrangement and kernel threads of xv6 operation system.
<p>
Each process can create and run at most 8 threads concurrently.
<p>
The VM layout now looks like the following

<p>
<pre>
+-------------------------------+  <--- USERTOP
|     Stack of the Process      |  1 PAGE
+-------------------------------+  
|      Guard Page (FREE)        |  1 PAGE
+-------------------------------+
|      Stack of Thread 1        |  1 PAGE
+-------------------------------+
|      Guard Page (FREE)        |  1 PAGE
+-------------------------------+
|                               |
:                               :
:              ^                :
|              |                |
+-------------------------------+  <--- proc->sz
|            Heap               | 
+-------------------------------+
|         Code + Data           |  1 PAGE
+-------------------------------+
|      Guard Page (FREE)        |  2 PAGES
+-------------------------------+
</pre>
