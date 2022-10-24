
# SOA
ENS HEM QUEDAT A Merge branch 'main' of github.com:hialvaro/ZeOS-SOA into main (OCT 27)
https://github.com/hialvaro/ZeOS-SOA/commits?before=da2160715d88cd53a9dcb3d6fb754316b0f8a4eb+35

## Comentaris del codi

### Coses no fetes
- Commits on Oct 27, 2021--> els dos de gettime no els hi hem fet ni cas, fa i desfa; wierd
- No fem el codi de kernel_utils.S **el necessitarem per sched_newt_rr** -- Adds task_switch function hialvaro committed on Nov 4, 2021

### Includes que per arreglar warnings
- A sched.c fem un include de interrupt.h perquè sinó ens salta un waning de implicit declaration de writeMSR a la linia 117


### Git commands
CLONE -> Bring a repository that is hosted somewhere like Github into a folder on your local machine (once per repo)

ADD -> Trak yout files and changes in Git

COMMIT -> Save your files in Git

PUSH -> Upload Git commits to a remot repo, like Github

PULL -> Download changes from a remote repo to your local machine (opposite of push)

STATUS -> shows all files that have been created, updated or deleted but haven't been saved in a commit yet

INIT -> Create a new Git repo from a folder in your local machine


EXAMPLES:

  git commit -m "Added readme.md" -m "further description if needed"
  
  git push origin master

### Install GIT

Steps in linux:

1) sudo apt-get update  

2) sudo apt-get install git  

3) git --version

4) git config --global user.name "UserName"

5) git config --global user.email "user@mail.com"

Further information:
-> https://www.atlassian.com/git/tutorials/install-git

### SSH keys

GENERATE A KEY: 

    ssh-keygen -t TYPE -b STRENGTH -C @EMAIL
    
    ssk-keygen -t rsa -b 4096 -C "email@example.com"

    - then we enter the name of the file we whant to save the key
    
    - we can optional enter a passphrase for owr key
    
    - your key is generated
    

    We will have two keys: 
    
      - id_rsa -> private key
      
      - id_rsa.pub -> public key

    - then we copy our public key
    
    - on GitHub we go to /settings/SSH and GPK keys we click on add a new ssh key
    
    - then we add owr SSH key to the ssh-agent following this steps:
    
    -> https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent#adding-your-ssh-key-to-the-ssh-agent 


For further explanations for Git BRANCHES check the video (32:42):

-> https://www.youtube.com/watch?v=RGOj5yH7evk&t=1509s&ab_channel=freeCodeCamp.org
