#include "main.h"
#include <dirent.h>
#include <sys/stat.h>

bool save_data() {
	DIR *dir = opendir("epic-shooter");
	if (!dir) {
		mkdir("epic-shooter", 0777);
	} else {
		closedir(dir);
	}


	FILE *fp = fopen("epic-shooter/save.bin", "w");
	if (!fp) {
		return false;
	}

	int ret = fseek(fp, 0, SEEK_SET);
	if (ret != 0) {
		return false;
	}

	for (int i = 0; i < num_levels; i++) {
		fwrite(&levels[i].high_score, sizeof(levels[i].high_score), 1, fp);
	}

	fclose(fp);
	return true;
}

bool load_data() {
	DIR *dir = opendir("epic-shooter");
	if (!dir) {
		return false;
	} else {
		closedir(dir);
	}

	FILE *fp = fopen("epic-shooter/save.bin", "r");
	if (!fp) {
		return false;
	}

	int ret = fseek(fp, 0, SEEK_SET);
	if (ret != 0) {
		return false;
	}

	for (int i = 0; i < num_levels; i++) {
		fread(&levels[i].high_score, sizeof(levels[i].high_score), 1, fp);
	}

	fclose(fp);
	return true;
}