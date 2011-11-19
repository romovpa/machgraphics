%% configuration
trainSetPath = 'train';

width = 80;

negativesPerImage = 2;

bootstrappingIters = 3;
detectionStep = 10;
maxIntersect = 20;
maxBootstrapPerImage = 5;

%% generating basic training set

% load data
trdata = load('-ASCII', fullfile(trainSetPath, 'train-processed.idl'));
imgIndexes = trdata(:, 1);
shifts = trdata(:,3);

X = [];
Y = [];
for i = 1:length(imgIndexes)
  imgIdx = imgIndexes(i);
  filename = sprintf('%d.png', imgIdx);
  img = imread(fullfile(trainSetPath, filename));
  
  fprintf('processing image %s\n', filename);
  
  % extract positive
  shift = shifts(i);
  f = extractWindowFeature(img(:, shift:shift+width));      % original
  X = [X; f'];
  Y = [Y; +1];
  
  %fr = extractWindowFeature(img(:, shift+width:-1:shift));  % reflected
  %X = [X; fr'];
  %Y = [Y; +1];
  
  % extract negatives
  for t = 1:negativesPerImage
    % choosing shift at random
    shift = fix(rand()*(size(img, 2)-width)) + 1;
    while (shift > shifts(i)-width) && (shift < shifts(i)+width)
      shift = fix(rand()*(size(img, 2)-width)) + 1;
    end
    
    f = extractWindowFeature(img(:, shift:shift+width));
    X = [X; f'];
    Y = [Y; -1];
  end
  
end

%% separating sets
set = 1:size(X,1); 
testRate = 1/3;
trainSet = 1:fix((1-testRate)*length(set));
validSet = fix((1-testRate)*length(set))+1:length(set);

%% cross validation C

C = 10.^(-5:2);
[model, acc] = trainSVMCrossval(X(trainSet,:), Y(trainSet), 5, C);
acc

%% accuracy on validation set
[acc, FP, FN] = classifierAccuracy(model, Y(validSet), X(validSet,:));
fprintf('Accuracy: %f\n', acc);
fprintf('FP: %d\n', FP);
fprintf('FN: %d\n', FN);

%% bootstrapping

models = {};
models{1} = model;

for iter = 2:bootstrappingIters
  
  fprintf('BOOTSTRAPPING, Iteration #%d\n', iter);
  
  % start detection
  for i = 1:length(imgIndexes)
    bsShifts = [];
    bsProb = [];
    
    imgIdx = imgIndexes(i);
    filename = sprintf('%d.png', imgIdx);
    img = imread(fullfile(trainSetPath, filename));
    
    fprintf('image %s: ', filename);
    
    shift = 1;
    while shift+width <= size(img,2)
      if ~(numel(intersect(shift:shift+width, shifts(i):shifts(i)+width)) > maxIntersect)
      
        window = img(:, shift:shift+width);
        f = extractWindowFeature(window);
        [decision, ~, prob] = predict(-1, sparse(f'), model, '');
        
        if decision > 0    % false positive
          bsShifts = [bsShifts shift];
          bsProb = [bsProb prob];
        end
      
      end
      shift = shift + detectionStep;
    end
    
    fprintf('%d FP''s', length(bsShifts));
    fprintf('\n');
    
    % bootstrap FP's
    [~,order] = sort(bsProb, 'descend');
    order = order(1:min(end:maxBootstrapPerImage));
    for j = 1:length(order)
      shift = bsShifts(order(j));
      window = img(:, shift:shift+width);
      f = extractWindowFeature(window);
      X = [X; f'];
      Y = [Y; -1];
      trainSet = [trainSet size(X, 1)];
    end
  end
  
  fprintf('New training set: %d objects', length(Y));
  
  % learning again
  [model, acc] = trainSVMCrossval(X(trainSet,:), Y(trainSet), 5, C);
  models{iter} = model;
  
end

%% test