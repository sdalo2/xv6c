#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "container.h"
#include "fs.h"


char* strcat(char* s1, const char* s2)
{
  char* b = s1;

  while (*s1) ++s1;
  while (*s2) *s1++ = *s2++;
  *s1 = 0;

  return b;
}

void copy_files(char* loc, char* src){
	int fd_write = open(loc, O_CREATE | O_RDWR);
	if(fd_write < 0){
		printf(1, "Invalid file location.\n");
		return;
	}

	int fd_read = open(src, O_RDONLY);
	if(fd_read < 0){
		printf(1, "Invalid file location.\n");
		return;
	}

	int bytes_read;
	char buf[512];

	while((bytes_read = read(fd_read, buf, sizeof(buf))) > 0){
		write(fd_write, buf, bytes_read);
	}
	close(fd_write);
	close(fd_read);
}

void init(){
	container_init();
}

void name(){
	char x[16], y[16], z[16], a[16];
	get_name(0, x);
	get_name(1, y);
	get_name(2, z);
	get_name(3, a);
	int b = get_curr_mem(0);
	int c = get_curr_mem(1);
	int d = get_curr_mem(2);
	int e = get_curr_mem(3);
	int s = get_curr_disk(0);
	printf(1, "0: %s - %d SIZE: %d, 1: %s - %d, 2: %s - %d, 3: %s - %d\n", x, b, s, y, c, z, d, a, e);
}

int
add_file_size_disk(char *path, char *c_name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  //int z;
  int holder = 0;

  if((fd = open(path, 0)) < 0){
    printf(2, "df: cannot open %s\n", path);
    return 0;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "df: cannot stat %s\n", path);
    close(fd);
    return 0;
  }

  switch(st.type){
  case T_FILE:
    holder += st.size;
    break;
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "df: cannot stat %s\n", buf);
        continue;
      }
      if(st.type == 1){
      	if(strcmp(de.name, "..") != 0 && strcmp(de.name, ".") != 0){
      		char *dir_name = strcat(de.name, "/");
      		holder += add_file_size_disk(dir_name, "");
      	}
      }
      if(strcmp(c_name, "") != 0){
	      int z = find(c_name);
	  	  if(z >= 0){
	  	  	int before = get_curr_disk(z);
		  	set_curr_disk(st.size, z);
		  	int after = get_curr_disk(z);
		  	if(before == after){
		  		cstop(c_name);
		  	}
		  }
		}
		holder += st.size;
    }
    break;
  }
  close(fd);
  printf(1, "SIZE %d\n", holder );
  return holder;
}

void
add_file_size(char *path, char *c_name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  int z;

  if((fd = open(path, 0)) < 0){
    printf(2, "df: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "df: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
  	printf(1, "%d \n", st.size);
  	if(strcmp(c_name, "") != 0){
	  	z = find(c_name);
	  	if(z >= 0){
	  		int before = get_curr_disk(z);
		  	set_curr_disk(st.size, z);
		  	int after = get_curr_disk(z);
		  	if(before == after){
		  		cstop(c_name);
		  	}
		}
	}
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      //printf(1, "DE name: %s\n DE type %d\n",de.type, de.tpye);
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "df: cannot stat %s\n", buf);
        continue;
      }
      //printf(1, "BLAH %d \n", st.size);
      if(strcmp(c_name, "") != 0){
	      int z = find(c_name);
	  	  if(z >= 0){
	  	  	int before = get_curr_disk(z);
		  	set_curr_disk(st.size, z);
		  	int after = get_curr_disk(z);
		  	if(before == after){
		  		cstop(c_name);
		  	}
		  }
		}
    }
    break;
  }
  close(fd);
}

void create(char *c_args[]){
	mkdir(c_args[0]);
	
	int x = 0;
	while(c_args[x] != 0){
			x++;
	}

	int i;

	for(i = 1; i < x; i++){
		char dir[strlen(c_args[0])];
		strcpy(dir, c_args[0]);
		strcat(dir, "/");
		char* location = strcat(dir, c_args[i]);
		copy_files(location, c_args[i]);
	}
}

void attach_vc(char* vc, char* dir, char* file[], int vc_num){
	int fd, id;
	fd = open(vc, O_RDWR);

	//TODO Check tosee file in file system
	char c_name[16];
	strcpy(c_name, dir);
	chdir(dir);
	// chroot(dir);

	/* fork a child and exec argv[1] */
	//tick_reset2();
	dir = strcat("/" , dir);
	add_file_size(dir, c_name);
	cont_proc_set(vc_num);
	id = fork();

	if (id == 0){
		close(0);
		close(1);
		close(2);
		dup(fd);
		dup(fd);
		dup(fd);
		printf(1, "FILE: %s\n", file[0]);
		exec(file[0], &file[0]);
		printf(1, "Failure to attach VC.");
		exit();
	}
}

void start(char *s_args[]){
	int index = 0;
	if((index = is_full()) < 0){
		printf(1, "No Available Containers.\n");
		return;
	}

	int x = 0;
	while(s_args[x] != 0){
			x++;
	}
	char* vc = s_args[0];
	char* dir = s_args[1];
	//char* file = s_args[2];

	if(find(dir) == 0){
		printf(1, "Container already in use.\n");
		return;
	}
	if(atoi(s_args[2]) != 0){ // proc
		set_max_proc(atoi(s_args[2]), index);
	}
	if(atoi(s_args[3]) != 0){ // mem 
		set_max_mem(atoi(s_args[3]), index);
	}
	if(atoi(s_args[4]) != 0){ // disk
		set_max_disk(atoi(s_args[4]), index);
	}
	set_name(dir, index);
	set_root_inode(dir);
	attach_vc(vc, dir, &s_args[5], index);
}

void cpause(char *c_name[]){
	pause(c_name[0]);
}

void cresume(char *c_name[]){ 
	resume(c_name[0]);
}

void stop(char *c_name[]){
	cstop(c_name[0]);
}

void info(){
	int num_c = max_containers();
	int i;
	for(i = 0; i < num_c; i++){
		char name[32];
		name[0] = '\0';
		get_name(i, name);
		if(strcmp(name, "") == 0){
			continue;
		}
		int m_used = get_curr_mem(i);
		int d_used = get_curr_disk(i);
		int p_used = get_curr_proc(i);
		int m_max = get_max_mem(i);
		int d_max = get_max_disk(i);
		int p_max = get_max_proc(i);
		printf(1, "Container: %s  Associated Directory: /%s\n", name , name);
		printf(1, "     Mem: %d used/%d available.\n", m_used, m_max);
		printf(1, "     Disk: %d used/%d available.\n", d_used, d_max);
		printf(1, "     Proc: %d used/%d available.\n", p_used, p_max);
		printf(1, "%s Processes\n", name);
		c_ps(name);
	}

}

int main(int argc, char *argv[]){
	if(strcmp(argv[1], "create") == 0){
		if(argc < 3){
			printf(1, "ctool create <name> <prog1> [ ... progn]\n");
			exit();
		}
		create(&argv[2]);
	}
	else if(strcmp(argv[1], "start") == 0){
		if(argc < 7){
			printf(1, "ctool start <vc> <name> <max_proc> <max_mem> <max_disk> <prog> [prog args]\n");
			exit();
		}
		start(&argv[2]);
	}
	else if(strcmp(argv[1], "name") == 0){
		name();
	}
	else if(strcmp(argv[1],"pause") == 0){
		if(argc < 2){
			printf(1, "ctool pause <name>\n");
			exit();
		}
		cpause(&argv[2]);
	}
	else if(strcmp(argv[1],"resume") == 0){
		if(argc < 2){
			printf(1, "ctool resume <name>\n");
			exit();
		}
		cresume(&argv[2]);
	}
	else if(strcmp(argv[1],"stop") == 0){
		if(argc < 2){
			printf(1, "ctool stop <name>\n");
			exit();
		}
		stop(&argv[2]);
	}
	else if(strcmp(argv[1],"info") == 0){
		info();
	}
	else{
		printf(1, "Improper usage; create, start, pause, resume, stop, info.\n");
	}
	exit();
}
