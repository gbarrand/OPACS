#ifndef HTCOMPRESSED_H
#define HTCOMPRESSED_H

#ifdef __cplusplus
extern "C"{
#endif
void HTCompressedFileToFile (char *fnam, int compressed);
void HTCompressedHText (HText *text, int compressed, int plain);
#ifdef __cplusplus
}
#endif

#endif /* not HTCOMPRESSED_H */
