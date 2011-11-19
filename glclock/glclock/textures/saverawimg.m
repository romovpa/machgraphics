function saverawimg(I, filename)

  f = fopen(filename, 'w');
  for i = 1:size(I,1)
    for j = 1:size(I,2)
      fwrite(f, I(i,j,:), 'uchar');
    end
  end
  fclose(f);

end