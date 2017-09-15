# Autonomous-car

This is the master repository for the autonomous car project of CMPE 243 under Prof. Preetpal Kang.

To start working on this project follow the steps mentioned below:

1) Fork this respository.

2) First download the repository to your machine either by clicking on green button saying "clone or download" and then download zip
  or 
copy the https:// path ( I prefer https:// over SSH because for SSH we need to follow a long procedure. I encourage you to learn it by your own with your personal repository. ) and do it by command line as mentioned below.

   Windows :
   - Download git bash ( https://git-scm.com/downloads ) and open git bash command window .
   - Use "ls" command to see files and directories present at the directory you are currently in. Change the present working directory by
     command "cd Directory_name" and  Go to the directory where you want to download the git repository. Use "cd .." to return to previous
     directory.
   - Then use "git clone PASTE_HTTPS://_PATH_HERE"
 
   Linux :
   - Write "git clone PASTE_HTTPS://_PATH_HERE" directly in linux terminal. Git is a tool which comes with the Linux.
 
   MAC :
   - Follow steps given on this site:  
     
     http://burnedpixel.com/blog/setting-up-git-and-github-on-your-mac/
     
     And then use git clone command as above. 

3) After cloning the repository on your machine you'll see all directories and files. 

4) Read, edit it and make some changes in related files while working on project. After this you have to upload the changes on remote
   repository.

   Follow this steps every time you make a change.
   - git add . (. means everything) "OR" git add -A  (-A means all. This will stage your changes ready to commit.)

   - git commit -m "write comments and mention your changes and why did you change it here. - your name." ( this will make it easy to see
     versions of our code and what new was added at what commit)

   - Use git checkout -b "name of branch you want to jump" . This is to upload files and not to mix everything in same branch.

   - Use git checkout "branch name you want to work on"  
"OR" git checkout -b "branch name" in case of creating new branch.

   - git push origin master  ( here master is a main branch. But when we will start working on it, we will make separate branch of each interface. For example there will a branch for sonar sensor called "sonar", or "motor" for all type of motor interface used in our project. In such case branch name will be "motor" or "sonar" instead of "master".

   - Now you can see your changes on website.
