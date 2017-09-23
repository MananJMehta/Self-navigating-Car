# Autonomous-car

This is the master repository for the autonomous car project of CMPE 243 under Prof. Preetpal Kang.

To start working on this project follow the steps mentioned below:

1) Fork this respository.

2) First download the repository to your machine either by clicking on green button saying "clone or download" and then download zip
   or 
   copy the https://<path> OR by command line as mentioned below.

   Windows :
   - Download git bash ( https://git-scm.com/downloads ) and open git bash command window .
   - Then use "git clone PASTE_HTTPS://_PATH_HERE"
 
   Linux :
   - Write "git clone PASTE_HTTPS://_PATH_HERE" direct in linux. Git is a tool which comes with the Linux.
 
   MAC :
   - Follow steps given on this site:  
     
     http://burnedpixel.com/blog/setting-up-git-and-github-on-your-mac/
     
     And then use git clone command as above. 

3) After cloning the repository on your machine you'll see all directories and files.
    - In this repository you won't see anything other then README because you are initally in master branch.
    - Use git branch to see the list or branches in this repository.
    - Use git checkout <branch name> to jump to other branch.
    - If you use ls command now then you'll see the directories in that branch.
  
  REMEMBER:
    - Every branch has data of all other branches created before it itself was created.
    - In our case Master was created first then Embedded and then Android.
    - So if you use ls command in master you'll see only README, if in Embedded branch then 5 modules + README, and in Android you'll see Android application code + 5 modules + README.

4) Read, edit it and make some changes in related files while working on project. After this you have to upload the changes on remote
   repository.

   Follow this steps every time you make a change.
   - git add . (. means everything) "OR" git add -A  (-A means all. This will stage your changes ready to commit.)
   - git add <file/folder name> works too.

   - git commit -m "write comments and mention your changes and why did you change it here. - your name." ( this will make it easy to see versions of our code and what new was added at what commit)

   - Make sure you change to the branch before you add and commit any change.

   - Use git checkout "branch name you want to work on"  
"OR" git checkout -b "branch name" in case of creating new branch.

   - git push origin <branch name>

   - Now you can see your changes on website.
