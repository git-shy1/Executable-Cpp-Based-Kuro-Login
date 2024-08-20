typedef char PACKAGENAME;
int killGG();
int getPID(char bm[64]);
int killprocess(char *bm);

int killprocess(PACKAGENAME * bm) {
	int pid = getPID(bm);
	if (pid == 0) {
		return -1;
	}
	char ml[32];
	sprintf(ml, "kill %d", pid);
	system(ml);					// 杀掉进程
	return 0;
}

int getPID(PACKAGENAME * PackageName) {
	DIR *dir = NULL;
	struct dirent *ptr = NULL;
	FILE *fp = NULL;
	char filepath[256];
	char filetext[128];
	dir = opendir("/proc");
	if (NULL != dir) {
		while ((ptr = readdir(dir)) != NULL) {
			if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
				continue;
			if (ptr->d_type != DT_DIR)
				continue;
			sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);
			fp = fopen(filepath, "r");
			if (NULL != fp) {
				fgets(filetext, sizeof(filetext), fp);
				if (strcmp(filetext, PackageName) == 0) {
					break;
				}
				fclose(fp);
			}
		}
	}
	if (readdir(dir) == NULL) {
		return 0;
	}
	closedir(dir);
	return atoi(ptr->d_name);
}

int killGG() {
	DIR* dir = NULL;
	DIR* dirGG = NULL;
	struct dirent* ptr = NULL;
	struct dirent* ptrGG = NULL;
	char filepath[256];			// The size is arbitrary, the path that can install the cmdline file can be
	char filetext[128];
	dir = opendir("/data/data");
	// puts("killing GG");
	int flag = 1;
	if (dir != NULL) {
		while (flag && (ptr = readdir(dir)) != NULL) {
			if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
				continue;
			if (ptr->d_type != DT_DIR)
				continue;
			sprintf(filepath, "/data/data/%s/files", ptr->d_name);
			dirGG = opendir(filepath);
			if (dirGG != NULL) {
				while ((ptrGG = readdir(dirGG)) != NULL) {
					if ((strcmp(ptrGG->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
						continue;
					if (ptrGG->d_type != DT_DIR)
						continue;
					if (strstr(ptrGG->d_name, Yohoho("GG"))) {
					    LogV(FAILED, "[ ! ] ⤷ GameGuardian Found. Please Uninstall Before Play!");
					    exit(0);
					}
				}
			}
		}
	}
	closedir(dir);
	closedir(dirGG);
	return 0;
}

