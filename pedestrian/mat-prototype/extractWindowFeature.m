function feature = extractWindowFeature(I)

hogopts.cellSize = [8 8];
hogopts.blockSize = [4 4];
hogopts.bins = 8;

feature = RHOG(I, hogopts);
%feature = vl_homkermap(feature, 1);

end