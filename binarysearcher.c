#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <ctype.h>
struct filename{
    char *file_name;
    int times;
};
struct binary_search_tree{

    char *word;
    int depth;
    struct binary_search_tree* left;
    struct binary_search_tree* right;
    struct filename* files;
    int totaltimes;
    int filesize;

};
typedef struct binary_search_tree binary_tree;
void setting_paths(binary_tree **,char *);/*i use 2 degree pointer for root thats why this functions are not binary_tree* */
void read_files(binary_tree **,char* );
void insert_binarytree(binary_tree **,char* ,char* );
void preorder(binary_tree *);
void ascending(binary_tree *);
void descending(binary_tree *);
void traverse_for_less_depth(binary_tree *);
int deleting(binary_tree**,char*);
int search(binary_tree**,char*);

int main(int argc,char *argv[])
{
    FILE *reader,*writer;
    char commands[20000];/*this is a something bad for coding but i could not figure how can i read with just pointer. I take too much because code might be stop*/
    char *command;
    int i;
    remove("output.txt");

    reader=fopen(argv[2],"r");

    char *filename=argv[1];
    binary_tree *root=NULL;
    int found=-1;
    setting_paths(&root,filename);

    while(fgets(commands,20000,reader)){
        writer=fopen("output.txt","a");
        command=strtok(commands, " \r\n");/*used string tokenizer for parsing commands*/
        if(command==NULL){
            continue;
        }
        printf("%s ",command);
        fprintf(writer,"%s ",command);
        if(!strcmp(command,"PRINT")){
            char *tree,*order;
            tree=strtok(NULL," \r\n");
            order=strtok(NULL," \r\n");

            if(order==NULL){/*in order*/
                printf("%s\n",tree);
                fprintf(writer,"%s\n",tree);
                fclose(writer);
                preorder(root);

                continue;
            }
            if(!strcmp(order,"DSC")){/*descending*/
                printf("%s %s\n",tree,order);
                fprintf(writer,"%s %s\n",tree,order);
                fclose(writer);
                descending(root);

            }
            if(!strcmp(order,"ASC")){/*ascending*/
                printf("%s %s \n",tree,order);
                fprintf(writer,"%s %s\n",tree,order);
                fclose(writer);
                ascending(root);
            }
        }
        if(!strcmp(command,"REMOVE")){/*remove*/
            char *will_delete;
            will_delete=strtok(NULL," \r\n");
            for(i = 0; i<strlen(will_delete); i++){
                will_delete[i] = tolower(will_delete[i]);
            }

            fclose(writer);
            found=deleting(&root,will_delete);
            writer=fopen("output.txt","a");
            printf("%s \n",will_delete);
            fprintf(writer,"%s \n",will_delete);
            fclose(writer);
        }
        if(!strcmp(command,"SEARCH")){/*Search*/
            char* will_search;
            will_search=strtok(NULL," \r\n");
            for(i = 0; i<strlen(will_search); i++){
                will_search[i] = tolower(will_search[i]);
            }
            printf("\n");
            fprintf(writer,"\n");
            fclose(writer);
            found=search(&root,will_search);
            writer=fopen("output.txt","a");
            if(found==0){/*thats mean not found so just wrote the word*/

                printf("%s\n",will_search);
                fprintf(writer,"%s\n",will_search);
            }
            fclose(writer);
        }
        if(!strcmp(command,"ADD")){/*Adding file*/
            char *path;
            path=strtok(NULL," \r\n");
            printf("%s\n",path);
            fprintf(writer,"%s\n",path);
            fclose(writer);
            read_files(&root,path);
        }

    }
    return 0;

}
int control_ascii(char* filename){/*control binary or not*/
    FILE* reader;
    int i,minus_values=0;
    char control[100];
    struct stat st;
    int size;
    stat(filename, &st);
    size = st.st_size;
    reader=fopen(filename,"rb");
    if(size<100){/*file can be smaller than 100 byte*/
        fread(control,sizeof(control[0]),size,reader);
        for(i=0;i<size;i++){
            if((0<control[i])&&(control[i]<128)){
                minus_values++;
            }
        }
        if(minus_values==size){
            return 1;
        }
        return 0;
    }
    fread(control,sizeof(control[0]),100,reader);
    for(i=0;i<100;i++){
        if((0<control[i])&&(control[i]<128)){
            minus_values++;
        }
    }
    if(minus_values==100){
        return 1;
    }
    return 0;

}
void setting_paths(binary_tree **root,char *filename){
    DIR *d;
    int i;
    d = opendir(filename);
    struct stat statBuffer;

    stat(filename, &statBuffer);

    while (1){
        struct dirent *entry;
        char *d_name;
        if(d==NULL){
            return;
        }
        entry=readdir(d);
        if(!entry){
            return;
        }
        d_name = entry->d_name;

        if(S_ISDIR(statBuffer.st_mode)){
            if((strcmp(entry->d_name, "..") != 0) && (strcmp(entry->d_name,".") != 0)) {/*do not take the directories . and ..*/
                char path[PATH_MAX];
                snprintf(path, PATH_MAX,"%s/%s", filename, d_name);


                int controller=0;
                for(i=2;i<strlen(path);i++){/*not start 0 because 0 will be dot*/
                    if(path[i]=='.'){/*if path has . it is a file*/
                        controller++;
                    }
                }
                if(controller!=0){/*do not take the path that has just directory. Like /user/something/something*/
                    read_files(root,path);
                }
                setting_paths(root,path);
            }
        }

    }

}
void read_files(binary_tree **root,char* filename){
    FILE *reader;
    int i;

    int controller=0;
    controller=control_ascii(filename);
    if(controller==0){/*controller==0 means file is binary so do not read*/
        return;
    }
    reader=fopen(filename,"r");

    char *string;
    int count=40960;
    char *belongfile;
    while(!feof(reader)){

        string=(char*)malloc(sizeof(char)*count*2);
        int j=0;
        int c=fscanf(reader,"%s",string);
        if(c==EOF){
            return;
        }
        if(c!=EOF){

            for(i=0;i<count;i++){/*take off the punctuation*/
                if(string[i]=='-'){/*except this -*/
                    string[j] =string[i];
                    j++;
                    continue;
                }
                if(!ispunct(string[i])){
                    string[j] =string[i];
                    j++;
                }

            }
            for(i = 0; i<strlen(string); i++){
                string[i] = tolower(string[i]);/*make upper case to lower*/
            }
            belongfile = strrchr(filename, '/');/*take the file's name from path*/
            belongfile=strtok(belongfile,"/\r\n");

            insert_binarytree(root,string,belongfile);

            free(string);

        }

    }

    return;

}
void insert_binarytree(binary_tree **root,char* data,char* belongfile){/*adding node to tree*/
    binary_tree *temp = *root, *prev = *root;
    int depth_counter=1;
    int i,j;

    if(*root==NULL){/*first node will be root*/
        *root = (binary_tree*)malloc(sizeof(binary_tree)*4);
        (*(*root)).word=(char*)malloc(sizeof(char*)*strlen(data)*2);
        (*(*root)).files=(struct filename*)malloc(sizeof(struct filename)*2);
        (*(*(*root)).files).file_name=(char*)malloc(sizeof(char*)*strlen(belongfile)*2);
        strcpy((*(*(*root)).files).file_name,belongfile);
        (*(*(*root)).files).times=1;
        (*(*root)).totaltimes=1;
        (*(*root)).filesize=1;
        (*(*root)).depth=1;
        (*(*root)).left=NULL;
        (*(*root)).right=NULL;
        strcpy((*(*root)).word,data);
        return;
    }else{

        while(temp!=NULL){
            depth_counter++;
            j=0;
            if(strcmp(data,(*temp).word)==0){/*if word already added*/

                for(i=0;i<(*temp).filesize;i++){
                   if(!strcmp((*temp).files[i].file_name,belongfile)){/*if word already added from same file*/
                        (*temp).files[i].times++;
                        (*temp).totaltimes++;
                        return;
                   }
                }
                for(i=0;i<(*temp).filesize;i++){
                    if(!strcmp((*temp).files[i].file_name,belongfile)){
                        j++;

                    }
                    if(j==0){/*j==0 means the word did not add from that file so add new file to word*/

                        (*temp).files[(*temp).filesize].file_name=(char*)malloc(sizeof(char*)*strlen(belongfile)*2);
                        strcpy((*temp).files[(*temp).filesize].file_name,belongfile);
                        (*temp).files[(*temp).filesize].times=1;
                        (*temp).totaltimes++;
                        (*temp).filesize++;
                        return;
                    }
                }

            }
            if(strcmp(data,(*temp).word)<0)
            {
                prev = temp;
                temp = (*temp).left;

            }
            else{
                prev = temp;
                temp = (*temp).right;

            }
        }

        binary_tree *temp_node = (binary_tree*)malloc(sizeof(binary_tree)*4);
        (*temp_node).word=(char*)malloc(sizeof(char*)*strlen(data)*2);
        (*temp_node).files=(struct filename*)malloc(sizeof(struct filename)*2);
        (*(*temp_node).files).file_name=(char*)malloc(sizeof(char*)*strlen(belongfile)*2);
        strcpy((*(*temp_node).files).file_name,belongfile);
        (*temp_node).totaltimes=1;
        (*temp_node).filesize=1;
        (*(*temp_node).files).times=1;
        (*temp_node).depth=depth_counter;
        (*temp_node).left=NULL;
        (*temp_node).right=NULL;
        strcpy((*temp_node).word,data);
        if (strcmp(data,(*prev).word)>0){
            (*prev).right = temp_node;
        }
        else{
            (*prev).left = temp_node;
        }
        return;
    }

}
void preorder(binary_tree *temp){/*preorder*/
    FILE *writer;

    int i;
    if (temp != NULL) {
        writer=fopen("output.txt","a");
        printf("%s\n", (*temp).word);
        printf("\t%d\n", (*temp).totaltimes);
        printf("\t%d\n", (*temp).depth);
        for(i=0;i<(*temp).filesize;i++){
            printf("\t%s ", (*temp).files[i].file_name);
            printf("%d\n",(*temp).files[i].times);
        }
        fprintf(writer,"%s\n", (*temp).word);
        fprintf(writer,"\t%d\n", (*temp).totaltimes);
        fprintf(writer,"\t%d\n", (*temp).depth);
        for(i=0;i<(*temp).filesize;i++){
            fprintf(writer,"\t%s ", (*temp).files[i].file_name);
            fprintf(writer,"%d\n",(*temp).files[i].times);
        }

        fclose(writer);
        preorder((*temp).left);
        preorder((*temp).right);
    }

}
void ascending(binary_tree *temp){/*ordered ascending*/
    FILE *writer;

    int i;
    if (temp != NULL) {
        ascending((*temp).left);
        writer=fopen("output.txt","a");
        printf("%s\n", (*temp).word);
        printf("\t%d\n", (*temp).totaltimes);
        printf("\t%d\n", (*temp).depth);
        for(i=0;i<(*temp).filesize;i++){
            printf("\t%s ", (*temp).files[i].file_name);
            printf("%d\n",(*temp).files[i].times);
        }
        fprintf(writer,"%s\n", (*temp).word);
        fprintf(writer,"\t%d\n", (*temp).totaltimes);
        fprintf(writer,"\t%d\n", (*temp).depth);
        for(i=0;i<(*temp).filesize;i++){
            fprintf(writer,"\t%s ", (*temp).files[i].file_name);
            fprintf(writer,"%d\n",(*temp).files[i].times);
        }
        fclose(writer);
        ascending((*temp).right);
    }

}
void descending(binary_tree *temp){/*ordered descending*/
    FILE *writer;

    int i;
    if (temp != NULL) {
        descending((*temp).right);
        writer=fopen("output.txt","a");
        printf("%s\n", (*temp).word);
        printf("\t%d\n", (*temp).totaltimes);
        printf("\t%d\n", (*temp).depth);
        for(i=0;i<(*temp).filesize;i++){
            printf("\t%s ", (*temp).files[i].file_name);
            printf("%d\n",(*temp).files[i].times);
        }
        fprintf(writer,"%s\n", (*temp).word);
        fprintf(writer,"\t%d\n", (*temp).totaltimes);
        fprintf(writer,"\t%d\n", (*temp).depth);
        for(i=0;i<(*temp).filesize;i++){
            fprintf(writer,"\t%s ", (*temp).files[i].file_name);
            fprintf(writer,"%d\n",(*temp).files[i].times);
        }
        fclose(writer);
        descending((*temp).left);

    }

}
int search(binary_tree **root,char* data){/*searching the word*/
    FILE *writer;
    writer=fopen("output.txt","a");
    binary_tree *temp=*root;
    int i;
    while (temp != NULL){
        if (strcasecmp(data,(*temp).word)==0) {
            printf("%s\n", (*temp).word);
            printf("\t%d\n", (*temp).totaltimes);
            printf("\t%d\n", (*temp).depth);
            for(i=0;i<(*temp).filesize;i++){
                printf("\t%s ", (*temp).files[i].file_name);
                printf("%d\n",(*temp).files[i].times);
            }
            fprintf(writer,"%s\n", (*temp).word);
            fprintf(writer,"\t%d\n", (*temp).totaltimes);
            fprintf(writer,"\t%d\n", (*temp).depth);
            for(i=0;i<(*temp).filesize;i++){
                fprintf(writer,"\t%s ", (*temp).files[i].file_name);
                fprintf(writer,"%d\n",(*temp).files[i].times);
            }
            fclose(writer);
            return 1;
        }


        if(strcmp(data,(*temp).word)<0){

            temp = (*temp).left;
        }
        else{
            temp = (*temp).right;
        }
   }

   return 0;

}
int deleting(binary_tree **root,char* data){
    binary_tree *temp=*root;
    binary_tree *prev=*root;
    while (temp != NULL){
        if (strcasecmp(data,(*temp).word)==0) {
            if ((*temp).left == NULL&&(*temp).right == NULL){/*Delete the node has no child*/
                if((*prev).left!=NULL){
                    if(!strcmp((*(*prev).left).word,(*temp).word)){/*if node is smaller than its parent*/
                        (*prev).left=NULL;
                        free(temp);
                        return 1;
                    }
                }
                if((*prev).right!=NULL){
                    if(!strcmp((*(*prev).right).word,(*temp).word)){/*if node is bigger than its parent*/
                        (*prev).right=NULL;
                        free(temp);
                        return 1;
                    }
                }
            }else if((*temp).left == NULL&&(*temp).right != NULL){/*if node has one child and has a child in right*/
                (*prev).right=(*temp).right;
                traverse_for_less_depth(temp);
                free(temp);
                return 1;
            }else if((*temp).left != NULL&&(*temp).right == NULL){/*if node has one child and has a child in left*/
                (*prev).left=(*temp).left;
                traverse_for_less_depth(temp);
                free(temp);
                return 1;
            }else if((*temp).left != NULL&&(*temp).right != NULL){/*if node has 2 child*/
                binary_tree *temp2=(*temp).right;
                binary_tree *prev2=temp2;/*last left's parent*/
                if((*temp2).left == NULL){/*node has 2 child but right child has no left child*/
                    traverse_for_less_depth(temp2);
                    (*temp2).left=(*temp).left;
                    (*temp).left=NULL;
                    if((*prev).left!=NULL){
                        if(!strcmp((*(*prev).left).word,(*temp).word)){
                            (*prev).left=temp2;

                        }
                    }
                    if((*prev).right!=NULL){
                        if(!strcmp((*(*prev).right).word,(*temp).word)){
                            (*prev).right=temp2;

                        }
                    }
                    if((*temp).depth==1){
                        *root=temp2;

                    }
                    free(temp);
                    return 1;
                }
                if((*temp2).left != NULL){/*delete right node's last left's*/
                    while ((*temp2).left!= NULL){
                        prev2=temp2;
                        temp2=(*temp2).left;
                    }
                    if((*temp2).right!=NULL){
                        (*prev2).left=(*temp2).right;
                    }else{
                        (*prev2).left=NULL;
                    }
                    (*temp2).left=(*temp).left;
                    (*temp2).right=(*temp).right;
                    if((*prev).left!=NULL){
                        if(!strcmp((*(*prev).left).word,(*temp).word)){
                            (*prev).left=temp2;

                        }
                    }
                    if((*prev).right!=NULL){
                        if(!strcmp((*(*prev).right).word,(*temp).word)){
                            (*prev).right=temp2;

                        }
                    }
                    if((*temp).depth==1){
                        *root=temp2;

                    }
                    free(temp);
                    return 1;
                }


            }

        }
        if(strcmp(data,(*temp).word)<0){
            prev = temp;
            temp = (*temp).left;
        }
        else{
            prev = temp;
            temp = (*temp).right;
        }
   }
    return 0;
}
void traverse_for_less_depth(binary_tree *temp) {/*after we deleting the node that has one child depth has to change its children*/

    if (temp != NULL) {
        traverse_for_less_depth((*temp).right);
        (*temp).depth=(*temp).depth-1;
        traverse_for_less_depth((*temp).left);

    }
}
