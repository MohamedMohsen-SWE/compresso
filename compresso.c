#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

struct node;
char codes[256][256];

typedef struct node {
    char c;
    int f;
    char code[256];
    int visited;
    struct node* left,*right;
} node;

size_t size=1024*sizeof(char);

char * getFileName(){
    char *fileName=NULL;
    printf("welcom to our compression tool\n");
    printf("please emnter the file name with its path\n");
    getline(&fileName,&size,stdin);
    fileName[strcspn(fileName,"\n")]='\0';
    return fileName;
}

//built tree
size_t huffManTreeSize;
node* builtHuffmanLeaves(int* arr,size_t arr_size){
  huffManTreeSize=1;
    node* huffmanTreeNodes=malloc(arr_size*sizeof(node));
    for(int i=0;i<arr_size;i++){
       

       if(arr[i]>0){
        
         huffmanTreeNodes[huffManTreeSize].c=(char)i;
   
       huffmanTreeNodes[huffManTreeSize].f=arr[i];
       huffmanTreeNodes[huffManTreeSize].left=NULL;
        huffmanTreeNodes[huffManTreeSize].right=NULL;
        huffmanTreeNodes[huffManTreeSize].code[0] = '\0';
        huffManTreeSize++;
    }
  }
    return huffmanTreeNodes;
}

//building heap start
void swap(node*x,node* y){
    node temp=*x; *x=*y; *y=temp;
}
void heapify(node* arr,int cur,size_t arr_size){
    int max=cur;
    int l=2*cur;
    int r=l+1;
    if(l<=arr_size&&arr[l].f>arr[max].f) max=l;
    if(r<=arr_size&&arr[r].f>arr[max].f) max=r;
    if(max==cur) return;
    swap(&arr[cur], &arr[max]);
    heapify(arr,max,arr_size);
}
void buildHeap(node *arr,size_t arr_size){
    for(int i=arr_size/2;i>0;i--) heapify(arr,i,arr_size);
}
void sortHeap(node * arr,size_t arr_size){
    for(int i=arr_size;i>0;i--){
        swap(&arr[1],&arr[i]);
        heapify(arr,1,i-1);
    }
}
//building heap end

//build Final tree start
node* buildFinalTree(node *arr,size_t size_arr){
    node* arrTree=malloc((sizeof(node)*(size_arr))*2);
   
    for(int i=1;i<=size_arr;i++){
        arrTree[i].c=arr[i].c;
        arrTree[i].f=arr[i].f;
        arrTree[i].left=arrTree[i].right=NULL;
        arrTree[i].code[0]='\0';
    }
    node *root;
    int i=0;
    node*parent;
    while(i<=size_arr){
        parent=malloc(sizeof(node));
        parent->left=malloc(sizeof(node));
       parent->right=malloc(sizeof(node));
        parent->f=(arrTree[i]).f+(arrTree[i+1]).f;
        parent->left->c=arrTree[i].c;
        parent->left->f=arrTree[i].f;
        parent->left->left=arrTree[i].left;
        parent->left->right=arrTree[i].right;
        parent->right->c=arrTree[i+1].c;
        parent->right->f=arrTree[i+1].f;
        parent->right->left=arrTree[i+1].left;
        parent->right->right=arrTree[i+1].right;
        parent->visited=0;
        //root=parent;
        arrTree[i+1]=*parent;
        
        buildHeap(arrTree,size_arr);
        sortHeap(arrTree,size_arr);
        i++;
    }
   
   
    return parent;
}
//build final tree end

//extracting codes using dfs start
void codeExtraction(node* root){
    node* arr_nodes[256];
    int top=-1;
    arr_nodes[++top]=root;
    root->visited=1;
    node* temp;
    root->code[0]='\0';
    while(top>=0){
        temp=arr_nodes[top];
        if(temp->left!=NULL&&temp->left->visited==0){
            temp->left->code[0]='\0';
            strcpy(temp->left->code, temp->code);
            strcat(temp->left->code, "0");
            temp = temp->left;
            arr_nodes[++top] = temp;
            temp->visited = 1;
        } else if(temp->right!=NULL&&temp->right->visited==0){
            temp->right->code[0]='\0';
            strcpy(temp->right->code, temp->code);
            strcat(temp->right->code, "1");
            temp = temp->right;
            arr_nodes[++top] = temp;
            temp->visited = 1;
        } else {
            if (temp->left==NULL && temp->right==NULL) {
                strcpy(codes[(unsigned char)temp->c], temp->code);
                //printf("%c -> %s\n", temp->c, temp->code);
            }
            top--;
        }
    }
}
//extracting codes using dfs end

//build Header start
void buildHeader(FILE* out){
  printf("\n\n");
    for(int i=0;i<256;i++){
        if(codes[i][0]!='\0'&&codes[i]!=NULL){
            fprintf(out,"%c : %s\n",i,codes[i]);
             //printf("%c -> %s\n",i,codes[i]);
        }
    }
     
    fprintf(out,"%s\n","end of the file");
}
//build header end

//start compression
void compress(FILE * in,FILE * out){
    int bitCount=0;
    unsigned int byte=0;
    unsigned int ch;
    rewind(in);
    fseek(out,0,SEEK_END);
    while((ch=fgetc(in))!=EOF){


        for(int i=0;codes[ch][i]!='\0';i++){
            byte<<=1;
            if(codes[ch][i]=='1') byte |=1;
            bitCount++;
            if(bitCount==8){
                fputc(byte,out);
                byte=0;
                bitCount=0;
            }
        }
    }
    if(bitCount>0){
      byte<<=(8-bitCount);
     fputc(byte,out);
      byte=0;
     bitCount=0;
}



}
//end compression

//start decompression
void decompress(FILE * file,FILE * out){
    char line[2048];
    char decodes[265][265];
    char sample;
    char* code;
    rewind(file);
    while (fgets(line, sizeof(line), file) != NULL ) {
        if (strcmp(line, "end of the file\n") == 0) {
            break;
        }
        if(line[0]=='\n'){
        if(fgets(line, sizeof(line), file) != NULL ){
        line[strlen(line)-1]='\0';
        sample='\n';

        code=line+3;
        strcpy(decodes[sample],code);
       
        }

        }else{

            line[strlen(line)-1]='\0';
        
         sample=line[0];
         code=line+4;
        strcpy(decodes[sample],code);
        }
      
    }
  
// for(unsigned int i=0;i<265;i++){
//   if(i=='\n'){
//     printf("%c -> %s\n",i,decodes[i]);
//   }


// }
unsigned int byte;
    char decoded[2048];
    decoded[0] = '\0';
    int pos = 0;

while ((byte=fgetc(file))!=EOF) {
 
        for (int i = 7; i >= 0; i--) {
            if (pos < 100) {
                int bit = (byte >> i) & 1;
                decoded[pos++] = (bit ? '1' : '0');
                decoded[pos] = '\0';
            }else{
                pos=0;
              decoded[pos] = '\0';
            }

            // check if current bitstring matches a code
            for (int j = 0; j < 256&&pos>2; j++) {
                if (decodes[j][0] != '\0' &&strcmp(decodes[j], decoded) == 0) {
                    
                    fputc((char)j, out);  
                  
                    decoded[0] = '\0';  
                    pos = 0;
                   
                    break;
                }
            }
        
        }
  }





}
//end decompression

//get file size
long  getFileSize(FILE*file){
    fseek(file,0,SEEK_END);
    long s=ftell(file);
    rewind(file);
    return s;
}
//end get file size
















void compressOption(char* inputFileName,char*outputFileName){

 const size_t chars_size=256;
    int chars[256]={0};
    FILE* input=fopen(inputFileName,"r");

long  originalSize=getFileSize(input);

    if(input==NULL){
        printf("file doesnt exits \n");
        return;
    }
    char* temp=(char*)malloc(size);
    while (fgets(temp,size,input)!=NULL) {
       for(int i=0;temp[i]!='\0';i++){
     chars[temp[i]]++;

        }
    }
  
  
    node* leaves=builtHuffmanLeaves(chars,chars_size);

    // for(int i=0;i<huffManTreeSize;i++){
    //     printf("%c %d \n",leaves[i].c,leaves[i].f);
    // }

     buildHeap(leaves,huffManTreeSize);
    
    // for(int i=0;i<huffManTreeSize;i++){
    //     printf("%c %d \n",leaves[i].c,leaves[i].f);
    // }
    sortHeap(leaves,huffManTreeSize);

     node* root=buildFinalTree(leaves,huffManTreeSize);
    codeExtraction(root);


   FILE *output=fopen(outputFileName,"wb");
  if(output==NULL){
       perror("cant open file\n");
       return ;
  }
buildHeader(output);
   compress(input,output);

 



long  compressedSize=getFileSize(output);
printf("the original size -> %.ld bytes",originalSize);
printf("\nthe compressed size -> %ld bytes",compressedSize);
float ratio = ((float)compressedSize / (float)originalSize) * 100.0;
printf("\nthe ration of compression  -> %.2f of the original size\n",ratio);
printf("============================================================================");



}


void decompressedOption(char *inputFileName,char*outputFileName){

printf("please wait ,loading..... ");
  FILE *input=fopen(inputFileName,"rb");
  if(input==NULL){
       perror("cant open file\n");
       return ;
  }


FILE *output=fopen(outputFileName,"w");
  if(output==NULL){


    perror("cant open file\n");
    return ;
   }

decompress(input,output);

fclose(input);
fclose(output);
  
}



void main(int argc , char* argv[argc]){
   



 
 int compressFlag = 0, decompressFlag = 0;
    char *inputFile = NULL, *outputFile = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) compressFlag = 1;
        else if (strcmp(argv[i], "-d") == 0) decompressFlag = 1;
        else if (strcmp(argv[i], "-o") == 0 && i+1 < argc) outputFile = argv[++i];
        else inputFile = argv[i];
    }

    if (compressFlag) {
      
       compressOption(inputFile,outputFile);

    } else if (decompressFlag) {
        decompressedOption(inputFile,outputFile);
    } else {
        printf("Usage: %s -c|-d <inputFile> -o <outputFile>\n", argv[0]);
        return ;
    }


}
