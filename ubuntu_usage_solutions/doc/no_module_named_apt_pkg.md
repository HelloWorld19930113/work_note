# `python-apt`出错
Traceback (most recent call last):  
  File "/usr/bin/apt-listchanges", line 28, in <module>  
    import apt_pkg  
ImportError: No module named apt_pkg  
  
sudo apt-get remove --purge python-apt  
sudo apt-get install python-apt